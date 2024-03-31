#include "logger.h"
#include "parser.h"
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

    struct parser_data *pdata = parser_parse(data);

    for (int i = 0; i < pdata->nheaders; i++) {
        printf("%s = %s\n", pdata->header_names[i], pdata->header_values[i]);
    }

    parser_free_data(pdata);

    free(data);

    // char *response = "HTTP/1.1 200\r\ncontent-type: text/html; charset=utf-8\r\ncontent-length: 0";

    // write(conn, response, strlen(response));

    close(conn);
    close(sockfd);

    return 0;
}
