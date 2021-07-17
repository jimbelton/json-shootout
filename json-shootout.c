#include <assert.h>
#include <cjson/cJSON.h>
#include <fcntl.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "dictionary-keys.h"

// install cJSON with: sudo apt install libcjson-dev
// build with: cc json-shootout.c -lcjson -o json-shootout

static double
doubletime(void)
{
    struct timeval now;

    assert(gettimeofday(&now, NULL) == 0);
    return now.tv_sec + now.tv_usec / 1000000.0;
}

static size_t
memory(void)
{
    struct mallinfo info = mallinfo();

    return info.uordblks;
}

int
main(void)
{
    struct stat stats[1];
    int         file;
    unsigned    i;
    char       *json;

    // Slurp the dictionary corpus
    assert((file = open("data/dictionary.json", O_RDONLY)) >= 0);
    assert(fstat(file, stats) >= 0);
    assert(json = malloc(stats->st_size + 1));
    assert(read(file, json, stats->st_size) == stats->st_size);
    json[stats->st_size] = '\0';

    cJSON *cjson;
    double startTime, duration;
    size_t startMem,  usedMem;

    startMem  = memory();
    startTime = doubletime();
    assert(cjson = cJSON_Parse(json));
    duration = doubletime() - startTime;
    usedMem  = memory()     - startMem;
    printf("Loaded dictionary in %fs, size %zukB\n", duration, usedMem / 1000);

    startTime = doubletime();

    for (i = 0; i < sizeof(dictionary_keys) / sizeof(dictionary_keys[0]); i++)
        assert(cJSON_GetObjectItem(cjson, dictionary_keys[i]));

    duration = doubletime() - startTime;
    printf("Looked up all keys in dictionary in %fs\n", duration);
    return 0;
}
