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
    printf(BOLD "--------------------\n"
                "name: " RESET "%s\n"
           BOLD "handle: " RESET "%s\n"
           BOLD "bio: " RESET "%s\n"
           BOLD "--------------------\n" RESET,
           self->name, self->handle, self->bio);
}