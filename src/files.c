#include "files.h"
#include "logger.h"

#include <stdio.h>
#include <stdlib.h>

char *read_file(const char *filename) {
    FILE *f = fopen(filename, "r");

    if (f == NULL) {
        logger_log(LOGGER_WARNING, "FILES", "Failed to open file");
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    int length = ftell(f);
    rewind(f);

    char *buffer = malloc(length + 1);

    if (buffer == NULL) {
        logger_log(LOGGER_CRITICAL, "FILES", "Failed to allocate file buffer");
        fclose(f);
        exit(EXIT_FAILURE);
    }

    int read_size = fread(buffer, 1, length, f);
    buffer[length] = '\0';

    fclose(f);

    if (length != read_size) {
        logger_log(LOGGER_WARNING, "FILES", "Failed to read file");
        free(buffer);
        return NULL;
    }

    return buffer;
}

// TODO: Implement path sanitisation
char *sanitise_path(const char *path) {
    return path;
}
