# json-shootout

JSON shootout is intended to be a better benchmark for C JSON implementations. The reason I felt the need to do this is that existing benchmarks seem to care only about the speed of parsing, and ignore the amount of memory used and the speed of access of the resulting data structures.

The baseline is written in Python 3. The benchmark parses a large corpus, measuring both time to do so and the amount of memory used to store the parsed data. It then looks up every member of the corpus, measuring the time required to do so.

## Corpus

The corpus is an English language dictionary created by Stackoverflow user Anurag Phadnis. See https://stackoverflow.com/questions/41768215/english-json-dictionary-with-word-word-type-and-definition#54982015


## Python Baseline (json-python.py)

Loaded dictionary in 0.68s, size 135872kB

Looked up all keys in dictionary in 0.025s

## cJSON (json-shootout)

Loaded dictionary in 0.22s, size 149548kB

Looked up all keys in dictionary in 625.54s

## Analysis

While cJSON parsed the corpus almost 3 times faster than Python, the resulting data structure was slightly larger. The real difference was in the speed of looking up that structure. Due to cJSON's horribly ineffecient data representation, its lookups where a whopping 25000 times slower.

## Next Steps

I plan to make the benchmark more extensive, including:
* doing more with the objects that are looked up, like counting how many of them are nouns
* benchmarking programmatic construction of a complex object
* benchmarking serialization of that object

I plan to add additional C JSON implementations, possibly starting with SIMDJSON, which makes some serious claims to being the fastest.
