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
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "repl.h"
#include "strtools.h"
#include "colors.h"

ReadEvalPrintLoop *repl_new(int number_of_commands) {
    ReadEvalPrintLoop *repl = (ReadEvalPrintLoop*) calloc(1, sizeof(ReadEvalPrintLoop));
    repl->commands = (REPLCommand*) calloc(number_of_commands, sizeof(REPLCommand));
    repl->max_command_count = number_of_commands;
    return repl;
}

GenericObject *generic_object_null() {
    GenericObject *generic_object = (GenericObject*) calloc(1, sizeof(GenericObject));
    return generic_object;
}

GenericObject *generic_object_string(char *object_string) {
    GenericObject *generic_object = (GenericObject*) calloc(1, sizeof(GenericObject));
    generic_object->string = object_string;
    return generic_object;
}

GenericObject *generic_object_user(char *object_user) {
    GenericObject *generic_object = (GenericObject*) calloc(1, sizeof(GenericObject));
    generic_object->user = object_user;
    return generic_object;
}

GenericObject *generic_object_hashtag(char *object_hashtag) {
    GenericObject *generic_object = (GenericObject*) calloc(1, sizeof(GenericObject));
    generic_object->hashtag = object_hashtag;
    return generic_object;
}

void repl_add_command_void(
    ReadEvalPrintLoop *repl,
    char *command,
    ObjectType callback_return_type,
    GenericObject *(*callback)(void)
) {
    if (repl->set_commands < repl->max_command_count) {
        REPLCommand repl_command = {
            .command = command,
            .callback_argument_type = REPL_NULL,
            .callback_return_type = callback_return_type,
            .callback_void = callback,
        };

        repl->commands[repl->set_commands] = repl_command;
        repl->set_commands += 1;
    } else {
        printf(BOLD RED "error: " RESET
               "repl has %d commands maximum, but tried to set %d\n",
                repl->max_command_count, repl->set_commands + 1);
        exit(1);
    }
}

void repl_add_command_requires_object(
    ReadEvalPrintLoop *repl,
    char *command,
    ObjectType callback_argument_type,
    ObjectType callback_return_type,
    GenericObject *(*callback)(GenericObject*)
) {
    if (repl->set_commands < repl->max_command_count) {
        REPLCommand repl_command = {
            .takes_value = true,
            .command = command,
            .callback_return_type = callback_return_type,
            .callback_argument_type = callback_argument_type,
            .callback_takes_value = callback,
        };

        repl->commands[repl->set_commands] = repl_command;
        repl->set_commands += 1;
    } else {
        printf(BOLD RED "error: " RESET
               "repl has %d commands maximum, but tried to set %d\n",
                repl->max_command_count, repl->set_commands);
        exit(1);
    }
}

void print_error_message(int token_offset, char *input, const char *error_message) {
    char spaces[token_offset];
    for(int i=0; i<token_offset; i++) {
        spaces[i] = ' ';
    }
    spaces[token_offset] = '\0';

    char *input_processed = get_string_at_index(input, 0, token_offset);
    char *input_errored = get_string_at_index(input, token_offset, strlen(input) - token_offset+1);

    printf(BOLD RED "error: " RESET "A problem occured.\n"
           "%s" BOLD RED "%s\n" RESET
           BOLD "%s~^" RESET " %s\n",
           input_processed, input_errored, spaces, error_message);
}

void repl_process_input(ReadEvalPrintLoop *repl, char *input) {
    int token_offset = 0;

    int input_len = strlen(input);
    char *input_copy = malloc(input_len);
    input_copy = strncpy(input_copy, input, input_len);

    GenericObject *(*function_to_execute)(GenericObject *);
    GenericObject *function_argument;

    ObjectType function_argument_type = REPL_NULL;
    int string_start_len[2];
    bool inside_string = false;

    int breakpoint = 0;
    bool executed_command = false;

    // tokenize copy of the string since we can't use the literal
    char *token = strtok(input_copy, " ");
    char *prev_token;
    while (token != NULL) {
        token_offset += strlen(token) + 1;

        if (function_argument_type == REPL_NULL) {
            for (int i = 0; i < repl->set_commands; i++) {
                if (!strcmp(repl->commands[i].command, get_string_at_index(input, 0, token_offset-1))) {
                    function_argument_type = repl->commands[i].callback_argument_type;
                    switch(function_argument_type) {
                        case REPL_STRING:
                            executed_command = true;
                            function_to_execute = repl->commands[i].callback_takes_value;
                        case REPL_TWEET:
                            executed_command = true;
                            break;
                        case REPL_USER:
                            executed_command = true;
                            break;
                        case REPL_HASHTAG:
                            executed_command = true;
                            break;
                        case REPL_NULL: {
                            if (token_offset >= strlen(input)) { // only execute if it's an exact match
                                (repl->commands[i].callback_void)();
                                executed_command = true;
                                goto execute_function;
                            } else {
                                breakpoint = token_offset;
                            }
                        } break;
                    }
                }
            }
        }
        else if (function_argument_type == REPL_STRING) {
            if (inside_string) {
                if (token[strlen(token) - 1] == '"') {
                    breakpoint = token_offset;
                    string_start_len[1] = token_offset - string_start_len[0] - 2;
                    char *string_object = get_string_at_index(input, string_start_len[0], string_start_len[1]);
                    function_argument = generic_object_string(string_object);
                    goto execute_function;
                }
            } else {
                if (token[0] == '"') {
                    string_start_len[0] = token_offset - strlen(token);
                    inside_string = true;
                    if (token[strlen(token) - 1] == '"') {
                        string_start_len[1] = token_offset - string_start_len[0] - 2;
                        breakpoint = token_offset;
                        char *string_object = get_string_at_index(input, string_start_len[0], string_start_len[1]);
                        function_argument = generic_object_string(string_object);
                        goto execute_function;
                    }
                }
                else
                    print_error_message(token_offset - strlen(token), input, "expected string in the form \"content\"");
            }
        }
        else if (function_argument_type == REPL_TWEET) {

        }
        else if (function_argument_type == REPL_USER) {
            if (token[0] == '@') {
                function_argument = generic_object_user(token);
                break;
            }
            else
                print_error_message(token_offset - strlen(token), input, "expected Twitter handle in the form \"@username\"");
        }
        else if (function_argument_type == REPL_HASHTAG) {
            if (token[0] == '#') {
                function_argument = generic_object_hashtag(token);
                break;
            }
            else
                print_error_message(token_offset - strlen(token), input, "expected hashtag in the form \"#hashtag\"");
        }
        prev_token = token;
        token = strtok(NULL, " ");
    }

execute_function:
    if (strlen(prev_token) > 0)
        token_offset = token_offset - strlen(prev_token) - 1;
    token = strtok(NULL, " ");
    if (token != NULL || (breakpoint > 0 && executed_command == false)) {
        print_error_message(breakpoint, input, "invalid options");
    }
    else if (function_argument_type != REPL_NULL && function_argument == NULL) {
        print_error_message(token_offset, input, "expected object");
    }
    else if (function_argument_type == REPL_NULL && (breakpoint == 0 && executed_command == false)) {
        print_error_message(0, input, "invalid command");
    }
    else if (function_argument_type != REPL_NULL) {
        (function_to_execute)(function_argument);
    }
}
