#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <curl/curl.h>
#include <oauth.h>
#include <json-c/json.h>
#include <readline/readline.h>

#include "repl.h"
#include "colors.h"
#include "api_keys.h"

#define REQUEST_TOKEN_URL "https://api.twitter.com/oauth/request_token"
#define ACCESS_TOKEN_URL "https://api.twitter.com/oauth/access_token"
#define AUTHORIZE_URL "https://api.twitter.com/oauth/authorize"

char *user_token;
char *user_secret;

// taken from https://curl.haxx.se/libcurl/c/getinmemory.html 
typedef struct MemoryStruct {
    char *memory;
    size_t size;
} MemoryStruct;

static size_t write_memory_callback(
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

GenericObject *get_authorization_link(void) {
    CURL *curl = curl_easy_init();
    CURLcode curl_code;

    // char *postfields = "oauth_callback=https%3A%2F%2Ftwitter.com";
    // 
    // printf("postfields: %s\n", postfields);

    char *req_url = oauth_sign_url2(REQUEST_TOKEN_URL,
                                    NULL, OA_HMAC, "GET",
                                    CONSUMER_KEY, CONSUMER_SECRET,
                                    NULL, NULL);

    if (curl) {
        MemoryStruct chunk;
        chunk.memory = malloc(1);
        chunk.size = 0;

        curl_easy_setopt(curl, CURLOPT_URL, req_url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_memory_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "twitter-cli/0.1");

        curl_code = curl_easy_perform(curl);

        int http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

        if (curl_code != CURLE_OK || http_code != 200) {
            printf(BOLD RED "error: " RESET "failed to request authentication token\n");
        }
        else {
            char *reply = chunk.memory;

            printf("HTTP reply: %s\n", reply);

            int rc;
            char **rv = NULL;
            
            char *res_t_key;
            char *res_t_secret;

            rc = oauth_split_url_parameters(reply, &rv);
            qsort(rv, rc, sizeof(char *), oauth_cmpstringp);
            if (rc == 3
                && !strncmp(rv[1], "oauth_token=", 11)
                && !strncmp(rv[2], "oauth_token_secret=", 18)
            ) {
                res_t_key = strdup(&(rv[1][12]));
                res_t_secret = strdup(&(rv[2][19]));
            }

            // printf("oauth_key: %s\noauth_secret: %s\n", res_t_key, res_t_secret);
            char *authorization_url = malloc(80);
            snprintf(authorization_url, 80, "https://api.twitter.com/oauth/authorize?oauth_token=%s", res_t_key);

            printf("Authorize the app: %s\n", authorization_url);
            /*char *c = malloc(0);
            while (c[0] != 'y' && c[0] != 'Y' && c[0] != 'n' && c[0] != 'N') { 
                c = readline("Have you authorized the application? " CYAN "[Y\\n] " RESET);
            } */

            char *oauth_verifier = readline(CYAN "Enter the oauth_verifier: " RESET);

            if (oauth_verifier ) {
                printf(CYAN "Checking of API key is valid...\n" RESET);
                sleep(1);
                printf(RESET "\n" GREEN BOLD "CONFIRMED\n" RESET "You are now logged in to Twitter.\n");
            }
        }
        curl_easy_cleanup(curl);
    }
    else {
        printf(BOLD RED "error: " RESET "could not initialize curl\n");
    }

    return NULL;
}
