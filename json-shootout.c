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
#include <sxe-hash.h>
#include <sxe-jitson.h>
#else
#include <cjson/cJSON.h>
#endif

#include "dictionary-keys.h"

#define USE_XXH3 1    // Define to use XXH3 hash for sxe-jitson

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

#ifdef USE_XXH3
#include <string.h>
#include <xxh3.h>

/**
 * Compute a hash sum of a fixed length or NUL terminated key using XX3 hash
 *
 * @param key    Pointer to the key
 * @param length Length of the key in bytes or 0 to use strlen
 *
 * @return 32 bit hash value
 */
unsigned
sxe_hash_xxh3(const void *key, unsigned length)
{
    if (length == 0)
        length = strlen(key);    /* COVERAGE EXCLUSION - Need a test */

    return XXH3_64bits(key, length);
}
#endif

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
#ifndef USE_XXH3
    sxe_hash_use_xxh32();
#else
    sxe_hash_override_sum(sxe_hash_xxh3);
#endif
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

#ifdef SXE_JITSON
    assert(sxe_jitson_object_get_member(jitson, dictionary_keys[0], 0));
#else
    assert(cJSON_GetObjectItem(cjson, dictionary_keys[0]));
#endif

    duration = doubletime() - startTime;
    usedMem  = memory()     - startMem;
    printf("First access in %fs, size %zukB\n", duration, usedMem / 1000);

    for (i = 1; i < sizeof(dictionary_keys) / sizeof(dictionary_keys[0]); i++)
#ifdef SXE_JITSON
        assert(sxe_jitson_object_get_member(jitson, dictionary_keys[i], 0));
#else
        assert(cJSON_GetObjectItem(cjson, dictionary_keys[i]));
#endif

    duration = doubletime() - startTime;
    printf("Looked up all keys in dictionary in %fs\n", duration);
    return 0;
}
