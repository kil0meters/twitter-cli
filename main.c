/*
 * A CLI client for Twitter.
 * Copyright (C) 2018 kilometers
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <readline/history.h>
#include <readline/readline.h>

#include "colors.h"
#include "user.h"
#include "twitter.h"
#include "gpl.h"

char *get_string_at_index(const char *src, int start, int len) {
    char *res = malloc(len + 1);
    memcpy(res, src + start, len);
    res[len + 1] = '\0';
    return res;
}

void print_help(void) {
    printf(
        BOLD "USAGE\n" RESET
        "   show me\n"
        "   show @elonmusk\n"
        BOLD "COMMANDS\n" RESET
        "   show     displays something on the screen\n"
        "   search   searches for a query\n"
        "   gpl      outputs information about the GNU Genneral Public License\n"
        "   help     shows this help message\n"
        "   exit     exits this application\n"
        BOLD "ALIASES\n" RESET
        "   trending gets the trending topics\n"
        "   @{}      gets information about the user with a Twitter handle\n"
        "   me       a shorthand for the account you are currently logged in as\n"
    );
}

char *cat_strs(const char *src, const char *str) {
    int bufsize = strlen(src) + strlen(str) + 1;
    char *res = malloc(bufsize);
    snprintf(res, bufsize, "%s%s", src, str);
    return res;
}

int main(int argc, char *argv[]) {

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

    struct stat st = {0};

    char *config_home = getenv("XDG_CONFIG_HOME");
    if (config_home == NULL) {
        config_home = cat_strs(getenv("HOME"), "/.config/twitter-cli");
    }
    if (stat(config_home, &st) == -1)
        mkdir(config_home, 0700);

    char *cache_home = getenv("XDG_CACHE_CONFIG");
    if (cache_home == NULL) {
        cache_home = cat_strs(getenv("HOME"), "/.cache/twitter-cli");
    }
    if (stat(cache_home, &st) == -1)
        mkdir(cache_home, 0700);

    char *history_file_name = cat_strs(cache_home, "/history.log");

    FILE *history_file = fopen(history_file_name, "r");
    if (history_file) {
        char buffer[256];
        while(fgets(buffer, 256, history_file)) {
            buffer[strcspn(buffer, "\n")] = 0;
            add_history(buffer);
        }
    }
    // flags, used to keep track of
    // what happened in previous iterations
    bool show;
    bool search;
    bool toplevel;
    bool inside_string;
    bool gpl;
    bool error;

    // used for error messages
    int token_offset;

    char *command;
    char *command_copy;

    int string_start_len[2]; // the text contained in a string
    char *output_queue;

    // read-eval-print loop
    while(true) {
        command = readline(BOLD "twitter" GREEN " ) " RESET);

        // add command to history
        FILE *history_file = fopen(history_file_name, "a");
        fprintf(history_file, cat_strs(command, "\n"));
        fclose(history_file);
        add_history(command);

        output_queue = "";

        token_offset = 0;

        // reset flags
        show = false;
        search = false;
        toplevel = true;
        inside_string = false;
        gpl = false;
        
        // create copy of command for error messages
        command_copy = malloc(strlen(command));
        command_copy = strncpy(command_copy, command, strlen(command));

        char *token = strtok(command, " ");
        while (token != NULL) {
            error = true;
            if(toplevel) {
                bool toplevel_complted = true;
                if (!strcmp(token, "help")) {
                    print_help();
                    break;
                } else if (!strcmp(token, "show")) {
                    show = true;
                } else if (!strcmp(token, "search")) {
                    search = true;
                } else if (!strcmp(token, "gpl")) {
                    output_queue = gpl_snippit();
                    gpl = true;
                } else if (!strcmp(token, "exit")) {
                    return 0;
                } else {
                    toplevel_complted = false;
                }

                if (toplevel_complted) {
                    toplevel = false;
                    token_offset += strlen(token) + 1;
                    token = strtok(NULL, " ");
                    
                    continue; // go to next token
                }
            }

            else if(gpl && error) {
                error = false;
                if (!strcmp(token, "warranty")) {
                    output_queue = gpl_warranty();
                } else if (!strcmp(token, "conditions")) {
                    output_queue = gpl_terms_and_conditions();
                } else {
                    error = true;
                }
            }

            else if(inside_string && error) {
                string_start_len[1] = token_offset + strlen(token) - string_start_len[0] - 1;
                error = false;

                if (token[strlen(token) - 1] == '"') {
                    inside_string = false;
                    char *string_in_quotes = get_string_at_index(command_copy, string_start_len[0], string_start_len[1]);

                    if (search) {
                        printf("Searching for: " BOLD CYAN "%s\n" RESET, string_in_quotes);
                    }
                }
            }

            else if (error) {
                error = false;
                if (!strcmp(token, "trending") && show) {
                    twitter_get_trending(user);
                } else if (!strcmp(token, "me") && show) {
                    user_print(user);
                } else if (token[0] == '@' && show) {
                    output_queue = "elon musk\n";
                } else if (token[0] == '"' && search) {
                    string_start_len[0] = token_offset + 1;
                    string_start_len[1] = strlen(token) - 2;
                    inside_string = true;
                } else {
                    error = true;
                }
            }
            
            if (error) {
                if(token_offset > strlen(command_copy))
                    token_offset = strlen(command_copy);
                // if(toplevel)
                //     token_offset = 0;

                // int space_count = token_offset - strlen(token);
                char spaces[token_offset + 1];
                for(int i=0; i<token_offset; i++) {
                    spaces[i] = ' ';
                }
                spaces[token_offset + 1] = '\0';

                char *command_processed = get_string_at_index(command_copy, 0, token_offset);
                char *command_errored = get_string_at_index(command_copy, token_offset, strlen(command_copy) - token_offset);

                printf(BOLD RED"error: " RESET "Invalid command\n"
                       "%s" BOLD RED "%s" RESET
                       BOLD "\n%s~^" RESET " encountered exception\n",
                       command_processed, command_errored, spaces);
                
                break;
            }

            token_offset += strlen(token) + 1;
            // go to next token, tracked in internal variable of strtok
            // who designed this function?
            token = strtok(NULL, " ");
        }

        if (!error)
            printf("%s", output_queue);
        free(command);
    }
    return 0;
}