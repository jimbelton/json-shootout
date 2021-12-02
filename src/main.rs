#[global_allocator]
static GLOBAL: jemallocator::Jemalloc = jemallocator::Jemalloc;

use simd_json::{self, ValueAccess};
use std::error::Error;
use std::time::Instant;

#[cfg(target_os = "linux")]
fn pid_res_usage_kb() -> u64 {
    probes::process_memory::current_rss().unwrap()
}

#[cfg(not(target_os = "linux"))]
fn pid_res_usage_kb() -> u64 {
    use libproc::libproc::pid_rusage::{pidrusage, PIDRUsage, RUsageInfoV0};

    match pidrusage::<RUsageInfoV0>(std::process::id() as i32) {
        Ok(res) => return res.memory_used() / 1024,
        Err(e) => {
            println!("Failed to retrieve RES memory for pid: {}", e);
            std::process::exit(1);
        }
    }
}

fn main() -> Result<(), Box<dyn Error>> {
    let mut data = std::fs::read("data/dictionary.json")?;

    // First test: load the data
    let start_mem = pid_res_usage_kb();
    let start_time = Instant::now();
    let json = simd_json::to_borrowed_value(&mut data)?;
    let duration = start_time.elapsed();
    let memory = pid_res_usage_kb() - start_mem;

    println!(
        "Loaded dictionary in {}s, size {}kB",
        duration.as_secs_f32(),
        memory,
    );

    // Second test: iterate through the records
    let start_time = Instant::now();
    if let Some(dictionary) = json.as_object() {
        for key in dictionary.keys() {
            let _ = dictionary[key];
        }
    }
    let duration = start_time.elapsed();

    println!(
        "Looked up all keys in dictionary in {}s",
        duration.as_secs_f32()
    );

    // Dump the JSON blob
    /*
    use serde_json::Value;
    let json_blob: Value = simd_json::serde::from_borrowed_value(json)?;
    println!("{}", json_blob);
    */

    Ok(())
}
