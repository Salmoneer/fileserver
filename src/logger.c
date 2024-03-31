#include "logger.h"

#include <stdio.h>
#include <stdlib.h>

void logger_log(enum logger_severity severity, const char *source, const char *message) {
    char *severity_tag;

    switch (severity) {
        case LOGGER_INFO:
            severity_tag = "INFO";
            break;
        case LOGGER_WARNING:
            severity_tag = "WARN";
            break;
        case LOGGER_ERROR:
            severity_tag = "EROR";
            break;
        case LOGGER_CRITICAL:
            severity_tag = "CRIT";
            break;
        default:
            printf("LOGGER: Failed to convert severity to string\n");
            exit(1);
    }

    printf("[%s] %s: %s\n", severity_tag, source, message);
}
