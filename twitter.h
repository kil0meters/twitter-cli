#ifndef TWITTER_H_
#define TWITTER_H_

#include "user.h"

typedef struct Tweet {
    char *name;
    char *handle;
    char *content;
    char *link_to_tweet;
} Tweet;

void twitter_get_trending(User *user);
char *twitter_search(char *search_string);

#endif