#include "response.h"
#include "server.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>

int sockfd;

void cleanup() {
    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);
}

int main() {
    sockfd = server_init(8069);

    atexit(cleanup);

    while (1) {
        int conn = accept(sockfd, NULL, NULL);

        char *data = server_read(conn);

        struct response_data *rdata = response_new(data);

        char *response = response_to_string(rdata);

        puts(response);

        write(conn, response, strlen(response));

        free(response);

        if (rdata != NULL) {
            response_free(rdata);
        }

        free(data);

        shutdown(conn, SHUT_RDWR);
        close(conn);
    }

    return 0;
}
