#!/usr/bin/env python3

import json;
import resource;
import time;

with open("data/dictionary.json") as fileStream:
    fileContent = fileStream.read()

startMem   = resource.getrusage(resource.RUSAGE_SELF).ru_maxrss
startTime  = time.time()
dictionary = json.loads(fileContent)
duration   = time.time() - startTime
memory     = resource.getrusage(resource.RUSAGE_SELF).ru_maxrss - startMem

print("Loaded dictionary in {}s, size {}kB".format(duration, memory));
