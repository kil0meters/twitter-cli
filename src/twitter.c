#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <oauth.h>
#include <curl/curl.h>
#include <readline/readline.h>
#include <json-c/json.h>

#include "xdg_dirs.h"
#include "strtools.h"
#include "repl.h"
#include "colors.h"
#include "api_keys.h"
#include "requests.h"
#include "twitter.h"
#include "server.h"

#define TWITTER_ACCESS_TOKEN_URL "https://api.twitter.com/oauth/access_token"
#define TWITTER_VERIFY_CREDENTIALS_URL "https://api.twitter.com/1.1/account/verify_credentials.json"

static void store_oauth_tokens(
    const char *oauth_token,
    const char *oauth_token_secret
) {
    char *fname = cat_strs(xdg_config_home(), "/twitter_authentication");
    FILE *twitter_oauth_file = fopen(fname, "w");

    if (twitter_oauth_file == NULL) {
        return; // silently fail for now
    }

    // write keys to file separated by newline
    fprintf(twitter_oauth_file, "%s\n%s\n",
            oauth_token,
            oauth_token_secret);

    fclose(twitter_oauth_file);
}

static const char **get_oauth_access_tokens(
    const char *oauth_key,
    const char *oauth_secret,
    const char *oauth_verifier
) {
    char *url = malloc(91);
    snprintf(url, 91, TWITTER_ACCESS_TOKEN_URL "?oauth_verifier=%s", oauth_verifier);

    char *postargs;
    oauth_sign_url2(url, &postargs, OA_HMAC,
                    "POST", CONSUMER_KEY, CONSUMER_SECRET,
                    oauth_key, oauth_secret);

    char *content = http_request(TWITTER_ACCESS_TOKEN_URL, postargs);

    const char **oauth_tokens = NULL;
    if (content != NULL) {
        oauth_tokens = malloc(2 * sizeof(char*));

        // parse reply
        int rc;
        char **rv = NULL;
        rc = oauth_split_url_parameters(content, &rv);
        qsort(rv, rc, sizeof(char *), oauth_cmpstringp);
        // printf("%s\n%s\n%s\n%s\n", rv[0], rv[1], rv[2], rv[3]);
        if (rc == 4
            && !strncmp(rv[0], "oauth_token=", 12)
            && !strncmp(rv[1], "oauth_token_secret=", 19)
        ) {
            oauth_tokens[0] = strdup(&(rv[0][12]));
            oauth_tokens[1] = strdup(&(rv[1][19]));
        }
    }

    return oauth_tokens;
}

TwitterUser *twitter_get_self(char *oauth_token, char *oauth_token_secret) {
    char *req_url = oauth_sign_url2(TWITTER_VERIFY_CREDENTIALS_URL,
                    NULL, OA_HMAC, "GET", CONSUMER_KEY,
                    CONSUMER_SECRET, oauth_token, oauth_token_secret);

    char *_content = http_request(req_url, NULL);

    json_object *content = json_tokener_parse(_content);

    json_object *name      = json_object_object_get(content, "name");
    json_object *username  = json_object_object_get(content, "screen_name");
    json_object *id        = json_object_object_get(content, "id");
    json_object *location  = json_object_object_get(content, "location");
    json_object *bio       = json_object_object_get(content, "description");
    json_object *url       = json_object_object_get(content, "expanded_url");
    json_object *followers = json_object_object_get(content, "followers_count");
    json_object *friends   = json_object_object_get(content, "friend_count");

    TwitterUser *user = malloc(sizeof(*user));
    user->name            = json_object_get_string(name);
    user->username        = json_object_get_string(username);
    user->id              = json_object_get_int(id);
    user->location        = json_object_get_string(location);
    user->bio             = json_object_get_string(bio);
    user->url             = json_object_get_string(url);
    user->followers_count = json_object_get_int(followers);
    user->friend_count    = json_object_get_int(friends);

    // any successful request shuold contain a username
    if (user->username == NULL)
        return NULL;

    return user;
}

GenericObject *authorize_account(void) {
    // start web server to manage oauth.
    if (server_start()) {
        printf(BOLD RED "error: " RESET "Could not start web server\n");
    }
    else {
        if (!generate_oauth_keys()) {
            const char *oauth_key = get_oauth_key();
            const char *oauth_secret = get_oauth_key_secret();

            char *authorization_url = malloc(80);
            snprintf(authorization_url, 80,
                     "https://api.twitter.com/oauth/authorize?oauth_token=%s", oauth_key);

            printf("Authorize the app: %s\n", authorization_url);

            const char *oauth_verifier = NULL;
            while (oauth_verifier == NULL) {
                oauth_verifier = get_oauth_verifier();
            }

            const char **oauth_tokens = get_oauth_access_tokens(oauth_key,
                                                                oauth_secret,
                                                                oauth_verifier);

            store_oauth_tokens(oauth_tokens[0], oauth_tokens[1]);
            printf(GREEN BOLD "CONFIRMED\n" RESET "You are now logged in to Twitter.\n");
        }
        else {
            printf(BOLD RED "error: " RESET "Could not generate oauth keys\n");
        }
        // stop web server
        server_stop();
    }
    return NULL;
}
