#include <assert.h>
#include <fcntl.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifdef SXE_JITSON
#include <sxe-jitson.h>
#else
#include <cjson/cJSON.h>
#endif

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

    return info.hblkhd + info.uordblks;
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

    double startTime, duration;
    size_t startMem,  usedMem;

    startMem = memory();    // Get memory before allocating any

#ifdef SXE_JITSON
    struct sxe_jitson_stack *stack;
    assert(stack = sxe_jitson_stack_new(2932736));
#endif

    startTime = doubletime();
#ifdef SXE_JITSON
    struct sxe_jitson *jitson;
    assert(sxe_jitson_stack_parse_json(stack, json));
    assert(jitson = sxe_jitson_stack_get_jitson(stack));
    printf("stack size = %zu\n", SXE_JITSON_TOKEN_SIZE * (size_t)stack->maximum);
    sxe_jitson_stack_free(stack);
#else
    cJSON *cjson;
    assert(cjson = cJSON_Parse(json));
#endif
    duration = doubletime() - startTime;
    usedMem  = memory()     - startMem;
    printf("Loaded dictionary in %fs, size %zukB\n", duration, usedMem / 1000);

    startTime = doubletime();

    for (i = 0; i < sizeof(dictionary_keys) / sizeof(dictionary_keys[0]); i++)
#ifdef SXE_JITSON
        assert(sxe_jitson_object_get_member(jitson, dictionary_keys[i], 0));
#else
        assert(cJSON_GetObjectItem(cjson, dictionary_keys[i]));
#endif

    duration = doubletime() - startTime;
    printf("Looked up all keys in dictionary in %fs\n", duration);
    return 0;
}
