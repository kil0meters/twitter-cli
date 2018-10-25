#include <stdlib.h>
#include <stdio.h>

#include <curl/curl.h>
#include <readline/readline.h>

#include "repl.h"
#include "colors.h"
#include "server.h"

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

            char *c = malloc(0);
            while (c[0] != 'y' && c[0] != 'Y' && c[0] != 'n' && c[0] != 'N') {
                c = readline("Have you authorized the application? " CYAN "[Y\\n] " RESET);
            }

            if (c[0] == 'y' || c[0] == 'Y') {
                const char *oauth_verifier = get_oauth_verifier();

                if (oauth_verifier == NULL) {
                    printf(BOLD RED "error: " RESET "twitter-cli was not properly authenticated\n");
                }
                else
                    printf(GREEN BOLD "CONFIRMED\n" RESET "You are now logged in to Twitter.\n");
            }
        }
        else {
            printf(BOLD RED "error: " RESET "Could not generate oauth keys\n");
        }
        // stop web server
        server_stop();
    }
    return NULL;
}
