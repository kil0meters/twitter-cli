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
#include "gpl.h"
#include "repl.h"
#include "strtools.h"
#include "twitter.h"
#include "server.h"

GenericObject *print_help(void) {
    printf(
        BOLD "COMMANDS\n" RESET
        "   search   searches for a query\n"
        "   login    needed to authenticate twitter-cli\n"
        "   gpl      outputs information about the GNU Genneral Public License\n"
        "   help     shows this help message\n"
        "   exit     exits this application\n"
        BOLD "ALIASES\n" RESET
        "   trending gets the trending topics\n"
        "   @{}      a Twitter user\n"
        "   #{}      a hashtag\n"
    );
    return NULL;
}

GenericObject *test_command(GenericObject *generic_string) {
    printf("%s", generic_string->string);
    return NULL;
}

GenericObject *twitter_search(GenericObject *generic_string) {
    printf("SEARCHING FOR: " BOLD CYAN "%s\n" RESET, generic_string->string);
    return NULL;
}

GenericObject *exit_application(void) {
    exit(0);
    return NULL;
}

int main(int argc, char *argv[]) {
    struct stat st = {0};

    /*char *config_home = getenv("XDG_CONFIG_HOME");
    if (config_home == NULL) {
        config_home = cat_strs(getenv("HOME"), "/.config/twitter-cli");
    }
    if (stat(config_home, &st) == -1)
        mkdir(config_home, 0700);*/

    char *cache_home = getenv("XDG_CACHE_CONFIG");
    if (cache_home == NULL) {
        cache_home = cat_strs(getenv("HOME"), "/.cache/twitter-cli");
    }
    if (stat(cache_home, &st) == -1)
        mkdir(cache_home, 0700);

    char *history_file_name = cat_strs(cache_home, "/history.log");

    free(cache_home);

    FILE *history_file = fopen(history_file_name, "r");
    if (history_file) {
        char buffer[256]; // probably large enough for now
        while(fgets(buffer, 256, history_file)) {
            buffer[strcspn(buffer, "\n")] = 0;
            add_history(buffer);
        }
    }
    fclose(history_file);

    ReadEvalPrintLoop *repl = repl_new(8);
    repl_add_command_requires_object(repl, "search", REPL_STRING, REPL_NULL, twitter_search);
    repl_add_command_void(repl, "server", REPL_NULL, start_server);
    repl_add_command_void(repl, "login", REPL_NULL, get_authorization_link);
    repl_add_command_void(repl, "help", REPL_NULL, print_help);
    repl_add_command_void(repl, "exit", REPL_NULL, exit_application); 
    repl_add_command_void(repl, "gpl", REPL_NULL, gpl_snippit);
    repl_add_command_void(repl, "gpl warranty", REPL_NULL, gpl_terms_and_conditions);
    repl_add_command_void(repl, "gpl conditions", REPL_NULL, gpl_warranty);

    // read-eval-print loop
    while(true) {
        char *command = readline(BOLD "twitter" GREEN " ) " RESET);
        
        if (command[0] != '\0') { // only process command if it's not empty
            // add command to history
            FILE *history_file = fopen(history_file_name, "a");
            fprintf(history_file, cat_strs(command, "\n"));
            fclose(history_file);
            
            add_history(command);

            repl_process_input(repl, command);
        }
        free(command);
    }
    free(repl);
    return 0; 
}
