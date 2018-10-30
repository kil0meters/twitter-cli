// WEB SERVER
//
// Since Twitter requires an OAuth verifier token,
// we need to temporarily run a web server in order to collect it
// (we set it to the callback url)


#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <curl/curl.h>
#include <json-c/json.h>
#include <oauth.h>
#include <microhttpd.h>

#include "repl.h"
#include "api_keys.h"
#include "colors.h"
#include "requests.h"

#define PORT 3456

#define REQUEST_TOKEN_URL "https://api.twitter.com/oauth/request_token"
#define ACCESS_TOKEN_URL "https://api.twitter.com/oauth/access_token"
#define AUTHORIZE_URL "https://api.twitter.com/oauth/authorize"

const char *oauth_key = NULL;
const char *oauth_key_secret = NULL;
const char *oauth_verifier = NULL;

struct MHD_Daemon *web_daemon;

// sets oauth_key and oauth_key_secret
// returns 1 if there was a failure
int generate_oauth_keys(void) {
    char *postargs = NULL;
    oauth_sign_url2(REQUEST_TOKEN_URL,
                    &postargs, OA_HMAC, "POST",
                    CONSUMER_KEY, CONSUMER_SECRET,
                    NULL, NULL);

    char *content = http_request(REQUEST_TOKEN_URL, postargs);

    if (content == NULL)
        return 1;

    int rc;
    char **rv = NULL;

    rc = oauth_split_url_parameters(content, &rv);
    qsort(rv, rc, sizeof(char *), oauth_cmpstringp);
    if (rc == 3
        && !strncmp(rv[1], "oauth_token=", 11)
        && !strncmp(rv[2], "oauth_token_secret=", 18)
    ) {
        oauth_key = strdup(&(rv[1][12]));
        oauth_key_secret = strdup(&(rv[2][19]));
    }

    return 0;
}

const char *get_oauth_key(void) {
    return oauth_key;
}
const char *get_oauth_key_secret(void) {
    return oauth_key_secret;
}
const char *get_oauth_verifier(void) {
    return oauth_verifier;
}

static int answer_to_connection(
    void *cls,
    struct MHD_Connection *connection,
    const char *url,
    const char *method,
    const char *version,
    const char *upload_data,
    size_t *upload_sata_size,
    void **con_cls
) {
    // printf("New %s request for %s using version %s\n", method, url, version);

    char *page = NULL;
    if (!strncmp(url, "/authentication/oauth_callback", 30)) {
        // ?oauth_token=AAAAAAAAAAAAAAAAAAAAAAAAAAA
        // &oauth_verifier=AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
        const char *_oauth_verifier;
        _oauth_verifier = MHD_lookup_connection_value(connection,
                                                      MHD_GET_ARGUMENT_KIND,
                                                      "oauth_verifier");
        // this is needed because _oauth_verifier gets overridden
        // upon the next request
        oauth_verifier = strdup(_oauth_verifier);
    }

    if (page == NULL)
        page = "twitter-cli v0.1";

    struct MHD_Response *response;

    response = MHD_create_response_from_buffer(strlen(page),
                                               (void*) page,
                                               MHD_RESPMEM_PERSISTENT);

    int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);

    return ret;
}

int server_start(void) {
    web_daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, PORT, NULL,
                                  NULL, &answer_to_connection, NULL,
                                  MHD_OPTION_END);
    if (web_daemon == NULL) {
        return 1;
    }

    return 0;
}

void server_stop(void) {
    MHD_stop_daemon(web_daemon);
}
