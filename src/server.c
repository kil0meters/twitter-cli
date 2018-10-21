// WEB SERVER
//
// Since Twitter requires an OAuth verifier token,
// we need to temporarily run a web server in order to collect it
// (we set it to the callback url)

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
// #include <sys/types.h>
// #include <sys/select.h>
// #include <sys/socket.h>
#include <microhttpd.h>

#include "repl.h"
#include "colors.h"

#define PORT 3456

int answer_to_connection(
    void *cls,
    struct MHD_Connection *connection,
    const char *url,
    const char *method,
    const char *version,
    const char *upload_data,
    size_t *upload_sata_size,
    void **con_cls
) {
    const char *page = "<html>Hello, browser!</html>";

    struct MHD_Response *response;

    response = MHD_create_response_from_buffer(strlen(page),
                                               (void*) page,
                                               MHD_RESPMEM_PERSISTENT);

    int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);

    return ret;
}

GenericObject *start_server(void) {
    struct MHD_Daemon *daemon;

    daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, PORT, NULL,
                              NULL, &answer_to_connection, NULL,
                              MHD_OPTION_END);
    if (daemon != NULL) {
        getchar();

        MHD_stop_daemon(daemon);
    } else {
        printf(BOLD RED "error: " RESET "Failed to initialize web server\n");
        exit(1);
    }
    
    return NULL;
}


