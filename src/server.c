#include "server.h"
#include "logger.h"

#include <poll.h>
#include <netinet/in.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define READ_SIZE 4096
#define TIMEOUT 10

const int yes = 1;

int server_init(int port) {
    logger_log(LOGGER_INFO, "SERVER", "Initializing listening socket");

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        logger_log(LOGGER_CRITICAL, "SERVER", "Failed to create listening socket");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
        logger_log(LOGGER_ERROR, "SERVER", "Failed to allow reusing addresses");
    }

    struct sockaddr_in addr = {.sin_family = AF_INET, .sin_port = htons(port), .sin_addr = {.s_addr = htonl(INADDR_ANY)}};

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr)) < 0) {
        logger_log(LOGGER_CRITICAL, "SERVER", "Failed to bind listening socket");
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, 1) < 0) {
        logger_log(LOGGER_CRITICAL, "SERVER", "Failed to listen on socket");
        exit(EXIT_FAILURE);
    }

    return sockfd;
}

char *server_read(int sockfd) {
    logger_log(LOGGER_INFO, "SERVER", "Reading request");

    char *buf = malloc(READ_SIZE);
    size_t buffer_size = READ_SIZE;
    size_t total_bytes_read = 0;
    ssize_t last_bytes_read;

    if (buf == NULL) {
        logger_log(LOGGER_CRITICAL, "SERVER", "Failed to allocate read buffer");
        exit(EXIT_FAILURE);
    }

    struct pollfd poll_fd = {.fd = sockfd, .events = POLLIN};

    do {
        if (poll(&poll_fd, 1, TIMEOUT) == 0) {
            break;
        }

        if (total_bytes_read + READ_SIZE > buffer_size) {
            buffer_size += READ_SIZE;
            buf = realloc(buf, buffer_size);

            if (buf == NULL) {
                logger_log(LOGGER_CRITICAL, "SERVER", "Failed to re-allocate read buffer");
                exit(EXIT_FAILURE);
            }
        }

        last_bytes_read = recv(sockfd, buf + total_bytes_read, READ_SIZE, 0);

        if (last_bytes_read < 0) {
            logger_log(LOGGER_CRITICAL, "SERVER", "Failed to read bytes from request");
            exit(EXIT_FAILURE);
        }

        total_bytes_read += last_bytes_read;
    } while (last_bytes_read > 0);

    buf = realloc(buf, total_bytes_read + 1);

    if (buf == NULL) {
        logger_log(LOGGER_ERROR, "SERVER", "Failed to re-allocate read buffer to resize");
    }

    buf[total_bytes_read] = '\0';

    return buf;
}
