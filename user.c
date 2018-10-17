#include <stdio.h>
#include <stdlib.h>

#include "user.h"
#include "colors.h"

User * user_new(
    char *name,
    char *handle,
    char *bio,
    char *link,
    int follow_count,
    int following_count,
    char *access_token
) {
    User *user = calloc(1, sizeof(User));

    user->name = name;
    user->handle = handle;
    user->bio = bio;
    user->link = link;
    user->follow_count = follow_count;
    user->following_count = following_count;
    user->access_token = access_token;

    return user;
}

void user_print(User *self) {
    printf("--------------------\n"
           BOLD "name: " RESET "%s\n"
           BOLD "handle: " RESET "%s\n"
           BOLD "bio: " RESET "%s\n"
           "--------------------\n",
           self->name, self->handle, self->bio);
}