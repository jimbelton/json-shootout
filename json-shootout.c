#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int
main(void)
{
    struct stat stats[1];
    int         file;
    char       *json;

    // Slurp the dictionary corpus
    assert((file = open("data/dictionary.json", O_RDONLY)) >= 0);
    assert(fstat(file, stats) >= 0);
    assert(json = malloc(stats->st_size + 1));
    assert(read(file, json, stats->st_size) == stats->st_size);
    json[stats->st_size] = '\0';

    fprintf(stderr, "About to dump %zu byte file contents\n", (size_t)stats->st_size);
    write(1, json, stats->st_size);
    return 0;
}
