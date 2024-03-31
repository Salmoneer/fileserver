#include "parser.h"
#include "logger.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

struct parser_data *parser_parse(const char *data) {
    struct parser p = {.data = data, .index = 0};

    struct parser_data *pdata = malloc(sizeof(struct parser_data));

    pdata->method   = parser_method(&p);
    pdata->path     = parser_until(&p, ' ');
    pdata->protocol = parser_until(&p, '\r');

    parser_headers(&p, pdata);

    return pdata;
}

void parser_free_data(struct parser_data *pdata) {
    free(pdata->path);
    free(pdata->protocol);

    for (int i = 0; i < pdata->nheaders; i++) {
        free(pdata->header_names[i]);
        free(pdata->header_values[i]);
    }

    free(pdata->header_names);
    free(pdata->header_values);

    free(pdata);
}

void parser_consume(struct parser *p) {
    while (isspace(p->data[p->index])) {
        p->index++;
    }
}

enum request_method parser_method(struct parser *p) {
    char *methods[] = {"GET", "HEAD", "POST", "PUT", "DELETE", "CONNECT", "OPTIONS", "TRACE", "PATCH"};

    for (int i = 0; i < sizeof(methods) / sizeof(*methods); i++) {
        if (memcmp(p->data + p->index, methods[i], strlen(methods[i])) == 0) {
            p->index += strlen(methods[i]);
            return i;
        }
    }

    logger_log(LOGGER_WARNING, "PARSER", "Malformed HTTP Request: Invalid protocol, assuming GET");
    return GET;
}

char *parser_until(struct parser *p, char c) {
    parser_consume(p);

    int length = 0;
    while (p->data[p->index + length] != c) {
        length++;
    }

    char *out = malloc(length + 1);

    if (out == NULL) {
        logger_log(LOGGER_CRITICAL, "PARSER", "Failed to allocate path string");
        exit(1);
    }

    memcpy(out, p->data + p->index, length);
    out[length] = '\0';

    p->index += length;

    return out;
}

void parser_headers(struct parser *p, struct parser_data *pdata) {
    parser_consume(p);

    pdata->nheaders = 0;
    pdata->header_names  = malloc(0);
    pdata->header_values = malloc(0);

    if (pdata->header_names == NULL || pdata->header_values == NULL) {
        logger_log(LOGGER_CRITICAL, "PARSER", "Failed to allocate headers");
        exit(EXIT_FAILURE);
    }

    while (!(p->data[p->index] == '\0' || (p->data[p->index] == '\r' && p->data[p->index + 1] == '\n'))) {
        pdata->nheaders++;

        char *name = parser_until(p, ':');
        p->index += 2; // ": "

        char *value = parser_until(p, '\r');
        p->index += 2; // "\r\n"

        pdata->header_names  = realloc(pdata->header_names,  pdata->nheaders * sizeof(char *));
        pdata->header_values = realloc(pdata->header_values, pdata->nheaders * sizeof(char *));

        if (pdata->header_names == NULL || pdata->header_values == NULL) {
            logger_log(LOGGER_CRITICAL, "PARSER", "Failed to allocate headers");
            exit(EXIT_FAILURE);
        }

        pdata->header_names [pdata->nheaders-1] = name;
        pdata->header_values[pdata->nheaders-1] = value;
    }
}
