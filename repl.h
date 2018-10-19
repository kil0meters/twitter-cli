#ifndef REPL_H_
#define REPL_H_

#include <stdbool.h>

typedef struct ReadEvalPrintLoop ReadEvalPrintLoop;
typedef struct REPLCommand REPLCommand;
typedef struct GenericObject GenericObject;
typedef enum ObjectType ObjectType;

struct ReadEvalPrintLoop {
    int max_command_count; // max number of commands
    int set_commands; // number of commands which are set
    REPLCommand *commands; // array of commands
};

struct GenericObject {
    char *string;
    char *user;
    char *hashtag;
};

enum ObjectType {
    REPL_STRING,
    REPL_TWEET,
    REPL_USER,
    REPL_HASHTAG,
    REPL_NULL,
};

struct REPLCommand {
    bool takes_value;
    char *command;
    ObjectType callback_argument_type;
    ObjectType callback_return_type;
    GenericObject *(*callback_void)(void);
    GenericObject *(*callback_takes_value)(GenericObject*);
};

GenericObject *generic_object_null();
GenericObject *generic_object_string(char *object_string);

ReadEvalPrintLoop *repl_new(int number_of_commands);

void repl_add_command_void(
    ReadEvalPrintLoop *repl,
    char *command,
    ObjectType callback_return_type,
    GenericObject *(*callback)(void)
);

void repl_add_command_requires_object(
    ReadEvalPrintLoop *repl,
    char *command,
    ObjectType callback_type,
    ObjectType callback_return_type,
    GenericObject *(*callback)(GenericObject*)
);

void repl_process_input(ReadEvalPrintLoop *repl, char *input);

#endif