#include "files.h"
#include "logger.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_file(char * __restrict filename) {
    FILE *f = fopen(filename, "r");

    free(filename);

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
char *sanitise_path(char *path) {
    char *prefix = "./content";

    if (strcmp(path, "/") == 0) {
        path = "/index.html";
    }

    char *out = malloc(strlen(prefix) + strlen(path) + 1);

    if (out == NULL) {
        logger_log(LOGGER_CRITICAL, "FILES", "Failed to allocate corrected path");
        exit(EXIT_FAILURE);
    }

    strcpy(out, prefix);
    strcat(out, path);

    puts(out);

    return out;
}
