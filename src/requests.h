#ifndef REQUESTS_H_
#define REQUESTS_H_

typedef struct MemoryStruct {
    char *memory;
    size_t size;
} MemoryStruct;

size_t write_memory_callback(
    void *contents,
    size_t size,
    size_t nmemb,
    void *userp
);

#endif
