#include "response.h"
#include "files.h"
#include "logger.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

// TODO: Refactor this function to a series of calls to simpler functions
struct response_data *response_new(const char *data) {
    struct parser_data *pdata = parser_parse(data);

    struct response_data *rdata = malloc(sizeof(struct response_data));

    if (rdata == NULL) {
        logger_log(LOGGER_CRITICAL, "RESPONSE", "Failed to allocate response data struct");
        exit(EXIT_FAILURE);
    }

    char *path = pdata->path;

    if (strcmp(path, "/") == 0) {
        path = "./index.html";
    }

    rdata->body = read_file(sanitise_path(path));

    response_protocol(pdata, rdata);
    response_status(rdata);
    response_headers(rdata);

    parser_free_data(pdata);

    return rdata;
}

void response_free(struct response_data *rdata) {
    free(rdata->protocol);
    free(rdata->status);

    for (int i = 0; i < rdata->nheaders; i++) {
        free(rdata->header_names[i]);
        free(rdata->header_values[i]);
    }

    free(rdata->header_names);
    free(rdata->header_values);

    free(rdata);
}

void response_add_header(struct response_data *rdata, const char *name, const char *value) {
    rdata->nheaders++;

    char *name_heap  = malloc(strlen(name)  + 1);
    char *value_heap = malloc(strlen(value) + 1);

    if (name_heap == NULL || value_heap == NULL) {
        logger_log(LOGGER_CRITICAL, "RESPONSE", "Failed to allocate header pair");
        exit(EXIT_FAILURE);
    }

    rdata->header_names =  realloc(rdata->header_names,  rdata->nheaders);
    rdata->header_values = realloc(rdata->header_values, rdata->nheaders);

    if (rdata->header_names == NULL || rdata->header_values == NULL) {
        logger_log(LOGGER_CRITICAL, "RESPONSE", "Failed to re-allocate header arrays");
        exit(EXIT_FAILURE);
    }

    strcpy(name_heap, name);
    strcpy(value_heap, value);
}

void response_protocol(struct parser_data *pdata, struct response_data *rdata) {
    if (strcmp(pdata->protocol, "HTTP/1.0") == 0 ||
        strcmp(pdata->protocol, "HTTP/1.1") == 0 ) {
        rdata->protocol = malloc(strlen(pdata->protocol) + 1);
        strcpy(rdata->protocol, pdata->protocol);
    } else {
        logger_log(LOGGER_WARNING, "RESPONSE", "Client sent request with protocol other than HTTP/1.0 or HTTP/1.1, assuming HTTP/1.1");
        rdata->protocol = malloc(strlen(pdata->protocol) + 1);
        strcpy(rdata->protocol, "HTTP/1.1");
    }
}

void response_status(struct response_data *rdata) {
    char *status;

    if (rdata->body == NULL) {
        logger_log(LOGGER_INFO, "RESPONSE", "Returning 404");
        rdata->code = 404;
        status = "File Not Found";
    } else {
        logger_log(LOGGER_INFO, "RESPONSE", "Returning 200");
        rdata->code = 200;
        status = "OK";
    }

    rdata->status = malloc(strlen(status) + 1);
    strcpy(rdata->status, status);
}

void response_headers(struct response_data *rdata) {
    rdata->nheaders = 0;
    rdata->header_names  = malloc(0);
    rdata->header_values = malloc(0);

    if (rdata->header_names == NULL || rdata->header_values == NULL) {
        logger_log(LOGGER_CRITICAL, "RESPONSE", "Failed to allocate headers");
        exit(EXIT_FAILURE);
    }

    int file_length = strlen(rdata->body);
    int length_str_length = ceil(log10(strlen(rdata->body))) + 1;
    char length_str[length_str_length];

    sprintf(length_str, "%d", file_length);

    response_add_header(rdata, "Content-Type", "text/html; charset=utf-8");
    response_add_header(rdata, "Content-Length", length_str);
}
