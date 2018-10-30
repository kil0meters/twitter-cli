#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <curl/curl.h>

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

char *http_request(
    const char *url,
    const char *postfields
) {
    CURL *curl;
    CURLcode res;

    MemoryStruct data = {
        .size = 0,
        .memory = malloc(0),
    };

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_memory_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "twitter-cli/0.1");

        if (postfields != NULL) {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postfields);
        }

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            printf(BOLD RED "error " RESET
                   "could not make request to Twitter's servers\n");
            return NULL;
        }
    }

    curl_easy_cleanup(curl);
    return data.memory;
}
