#[global_allocator]
static GLOBAL: jemallocator::Jemalloc = jemallocator::Jemalloc;

use libproc::libproc::pid_rusage::{pidrusage, PIDRUsage, RUsageInfoV0};
use simd_json::{self, ValueAccess};
use std::error::Error;
use std::process;
use std::time::Instant;

fn pid_res_usage_kb() -> u64 {
    let pid = process::id() as i32;
    if let Ok(res) = pidrusage::<RUsageInfoV0>(pid) {
        return res.memory_used() / 1024;
    }

    println!("Failed to get RES memory size for pid: {}", pid);
    process::exit(1);
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
        memory
    );

    // Second test: iterate through the records
    let start_time = Instant::now();
    if let Some(dictionary) = json.as_object() {
        // We could use .iter() or .values() here instead of .keys()
        // but I wanted to match the python script.
        for key in dictionary.keys() {
            assert!(dictionary.get(key).is_some());
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
