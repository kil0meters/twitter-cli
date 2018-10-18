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

#include <json-c/json.h>
#include <curl/curl.h>

#include "user.h"
#include "twitter.h"
#include "colors.h"

// ┌─┬─┐
// │ │ │
// ├─┼─┤
// └─┴─┘

int twitter_oauth_consumer(int use_post) {
    return 0;
}

bool test_api_key_fails(const char *api_key) {
    return false;
}

void twitter_get_trending(User *user) {
    CURL *curl = curl_easy_init();

    // char *url = "";
    if(curl) {

        curl_easy_setopt(curl, CURLOPT_URL, "https://twitter.com");
    }
}

char *twitter_search(char *search_string) {
    int bufsize = 41 + strlen(search_string);
    char *search = malloc(bufsize);
    snprintf(search, bufsize, "Searching for: " BOLD CYAN "%s\n" RESET, search_string);
    return search;
}