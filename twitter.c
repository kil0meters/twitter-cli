#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include <readline/history.h>
#include <readline/readline.h>
#include <json-c/json.h>
#include <curl/curl.h>

#include "user.h"
#include "colors.h"

typedef struct Tweet {
    char *name;
    char *handle;
    char *content;
    char *link_to_tweet;
} Tweet;

bool test_api_key_fails(const char *api_key) {
    return false;
}

void twitter_get_trending(User *user) {
    CURL *curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://twitter.com");
    }
}

void print_help(void) {
    printf(
        BOLD "EXAMPLE USAGE\n" RESET
        "   show me\n"
        "   show @elonmusk\n"
        BOLD "COMMANDS\n" RESET
        "   show     displays something on the screen\n"
        "   search   searches for a query\n"
        "   help     shows this help message\n"
        "   exit     exits this application\n"
        BOLD "ALIASES\n" RESET
        "   trending gets the trending topics\n"
        "   @{}      gets information about the user with a Twitter handle\n"
        "   me       a shorthand for the account you are currently logged in as\n"
    );
}

int main(int argc, char *argv[]) {
    /* char *api_key = readline(BOLD "Enter a Twitter API key: " RESET);
    if(test_api_key_fails(api_key)) {
        fprintf(stderr, RED BOLD "error: " RESET "Invalid API key\n");
        return 1;
    } */

    // create user instance and set access token for development
    User *user = user_new(
        "kilometers",
        "ki10meters",
        "biography",
        "https://meters.sh",
        14,
        89,
        "asdfasdf"
    );

    // flags, used to keep track of
    // what happened in previous iterations
    bool show = false;

    // read-eval-print loop
    char *command;
    while(true) {
        command = readline(BOLD "twitter> " RESET);
        add_history(command);

        // reset flags
        show = false;
        
        char *token = strtok(command, " ");
        while (token != NULL) {
            //printf("\"%s\"\n", token);
            if (!strcmp(token, "help")) {
                print_help();
            } else if (!strcmp(token, "me")) {
                if(show)
                    user_print(user);
            } else if (!strcmp(token, "show")) {
                show = true;
            } else if (!strcmp(token, "search")) {

            } else if (token[0] == '@') {
                if (show)
                    printf("elon musk\n");
            }

            // go to next token, tracked in internal variable of strtok
            // who designed this function?
            token = strtok(NULL, " ");
        }

        free(command);
    }
    return 0;
}