package main

import "encoding/json"
import "fmt"
import "io/ioutil"
import "runtime"
import "time"

func main() {
    fileContent, err := ioutil.ReadFile("data/dictionary.json")

    if (err != nil) {
        panic(err.Error())
    }

    var memStats runtime.MemStats
    runtime.ReadMemStats(&memStats)
    startMem  := memStats.HeapAlloc
    startTime := time.Now()
    dictionary := map[string]interface{}{}
    json.Unmarshal(fileContent, &dictionary)
    elapsed := time.Now().Sub(startTime)
    runtime.ReadMemStats(&memStats)
    memory := memStats.HeapAlloc - startMem
    fmt.Printf("Loaded dictionary in %vs, size %vkB\n", elapsed.Seconds(), memory / 1024)

    keys := make([]string, 0, len(dictionary))

    for key := range dictionary {
        keys = append(keys, key)
    }

    startTime = time.Now()

    for _, value := range keys {
        _ = dictionary[value]
    }

    elapsed = time.Now().Sub(startTime)
    fmt.Printf("Looked up all keys in dictionary in %vs\n", elapsed.Seconds());
}
