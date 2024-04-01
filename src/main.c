#include "response.h"
#include "server.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>

int main() {
    int sockfd = server_init(8069);

    int conn = accept(sockfd, NULL, NULL);

    char *data = server_read(conn);

    struct response_data *rdata = response_new(data);

    response_free(rdata);

    free(data);

    close(conn);
    close(sockfd);

    return 0;
}
