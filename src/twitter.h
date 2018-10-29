#ifndef TWITTER_H_
#define TWITTER_H_

#include "repl.h"

typedef struct TwitterUser {
    const char *name;
    const char *username;
    int id;
    const char *location;
    const char *bio;
    const char *url;
    int followers_count;
    int friend_count; // number of people the user is following
} TwitterUser;

TwitterUser *twitter_get_self(char *oauth_token, char *oauth_token_secret);
GenericObject *authorize_account(void);

#endif
