#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "colors.h"
#include "requests.h"

size_t write_memory_callback(
    void *contents,
    size_t size,
    size_t nmemb,
    void *userp
) {
    size_t realsize = size * nmemb;
    MemoryStruct *mem = (MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if (ptr == NULL) {
        // this should never happen -- out of memory
        printf(BOLD RED "error: " RESET "OUT OF MEMORY.\n");
        exit(1);
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

