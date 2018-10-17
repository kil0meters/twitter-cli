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

// ┌─┬─┐
// │ │ │
// ├─┼─┤
// └─┴─┘


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
        BOLD "USAGE\n" RESET
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
    bool show;
    bool toplevel;

    // used for error messages
    int token_offset;

    char *command;
    char *command_copy;

    // read-eval-print loop
    while(true) {
        command = readline(BOLD "twitter" GREEN " ) " RESET);
        add_history(command);

        token_offset = 0;

        // reset flags
        show = false;
        toplevel = true;
        
        // create copy of command for error messages
        command_copy = (char*) malloc(strlen(command));
        command_copy = strncpy(command_copy, command, strlen(command));

        char *token = strtok(command, " ");
        while (token != NULL) {
            if(toplevel) {
                bool executed_command = true;
                if (!strcmp(token, "help")) {
                    print_help();
                    break;
                } else if (!strcmp(token, "show")) {
                    show = true;
                } else if (!strcmp(token, "search")) {
                } else {
                    executed_command = false;
                }

                if (executed_command) {
                    toplevel = false;
                    token_offset += strlen(token) + 1;
                    token = strtok(NULL, " ");
                    
                    continue; // go to next token
                }
            }

            printf("token offset: %d\n", token_offset);

            if (!strcmp(token, "trending") && show) {
                printf("trending\n");
                // break;
            } else if (!strcmp(token, "me") && show) {
                user_print(user);
            } else if (token[0] == '@' && show) {
                printf("elon musk\n");
            } else {
                if(token_offset > strlen(command_copy))
                    token_offset = strlen(command_copy);
                if(toplevel)
                    token_offset = 0;
                
                

                // int space_count = token_offset - strlen(token);
                char spaces[token_offset + 1];
                for(int i=0; i<token_offset; i++) {
                    spaces[i] = ' ';
                }
                spaces[token_offset + 1] = '\0';

                char *command_processed = (char*) malloc(token_offset);
                memcpy(command_processed, command_copy, token_offset);
                command_processed[token_offset] = '\0';

                int command_errored_len = strlen(command_copy) - token_offset;
                char *command_errored = (char*) malloc(command_errored_len);
                memcpy(command_errored, command_copy + token_offset, command_errored_len);
                command_errored[command_errored_len] = '\0';

                printf(BOLD RED"error: " RESET "Invalid command\n"
                       "%s" BOLD RED "%s" RESET
                       BOLD "\n%s~^" RESET " encountered error\n",
                       command_processed, command_errored, spaces);
                
                break;
            }

            token_offset += strlen(token) + 1;
            // go to next token, tracked in internal variable of strtok
            // who designed this function?
            token = strtok(NULL, " ");
        }

        free(command);
    }
    return 0;
}