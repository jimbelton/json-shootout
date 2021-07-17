#!/usr/bin/env python3

import argparse;
import json;
import resource;
import time;

parser = argparse.ArgumentParser(description='Python baseline for JSON benchmark')
parser.add_argument('--header', action='store_true')
args = parser.parse_args()

with open("data/dictionary.json") as fileStream:
    fileContent = fileStream.read()

startMem   = resource.getrusage(resource.RUSAGE_SELF).ru_maxrss
startTime  = time.time()
dictionary = json.loads(fileContent)
duration   = time.time() - startTime
memory     = resource.getrusage(resource.RUSAGE_SELF).ru_maxrss - startMem

print("Loaded dictionary in {}s, size {}kB".format(duration, memory));

startTime  = time.time()

for key in dictionary.keys():
    assert dictionary.get(key)

duration   = time.time() - startTime

print("Looked up all keys in dictionary in {}s".format(duration));

if args.header:
    with open("dictionary-keys.h", "w") as fileStream:
        print("char *dictionary_keys[] = {", file=fileStream)

        for key in dictionary.keys():
            print("    \"{}\",".format(key), file=fileStream)

        print("};", file=fileStream)
