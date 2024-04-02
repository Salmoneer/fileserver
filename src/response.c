#include "response.h"
#include "files.h"
#include "logger.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

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

    if (rdata->body == NULL) {
        logger_log(LOGGER_WARNING, "RESPONSE", "Sending 404");
        response_404(rdata);
        parser_free_data(pdata);
        return rdata;
    }

    int i = strlen(pdata->path);

    while (i > 0 && pdata->path[i - 1] != '.') {
        i--;
    }

    enum content_type ctype;

    if (strcmp(pdata->path + i, "html") == 0) {
        ctype = CONTENT_HTML;
    } else if (strcmp(pdata->path + i, "css") == 0) {
        ctype = CONTENT_CSS;
    } else {
        logger_log(LOGGER_ERROR, "RESPONSE", "Failed to detect content type from file extension, assuming html");
        ctype = CONTENT_HTML;
    }

    response_protocol(pdata, rdata);
    response_status(rdata);
    response_headers(rdata, ctype);

    parser_free_data(pdata);

    return rdata;
}

void response_404(struct response_data *rdata) {
    char *protocol = "HTTP/1.1";
    rdata->protocol = malloc(strlen(protocol) + 1);
    strcpy(rdata->protocol, protocol);

    rdata->code = 404;

    char *status = "File Not Found";
    rdata->status = malloc(strlen(status) + 1);
    strcpy(rdata->status, status);

    char *body = "404: File Not Found";
    rdata->body = malloc(strlen(body) + 1);
    strcpy(rdata->body, body);

    response_headers(rdata, CONTENT_HTML);
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

    free(rdata->body);

    free(rdata);
}

char *response_to_string(struct response_data *rdata) {
    if (rdata == NULL) {
        char *out = malloc(1);
        out[0] = '\0';
        return out;
    }

    char *out = malloc(1);

    if (out == NULL) {
        logger_log(LOGGER_CRITICAL, "RESPONSE", "Failed to allocate response string");
        exit(EXIT_FAILURE);
    }

    out[0] = '\0';

    int str_length = ceil(log10(rdata->code)) + 1;
    char code[str_length];

    sprintf(code, "%d", rdata->code);

    out = response_string_extend(out, rdata->protocol);
    out = response_string_extend(out, " ");
    out = response_string_extend(out, code);
    out = response_string_extend(out, " ");
    out = response_string_extend(out, rdata->status);
    out = response_string_extend(out, "\r\n");

    for (int i = 0; i < rdata->nheaders; i++) {
        out = response_string_extend(out, rdata->header_names[i]);
        out = response_string_extend(out, ": ");
        out = response_string_extend(out, rdata->header_values[i]);
        out = response_string_extend(out, "\r\n");
    }

    out = response_string_extend(out, "\r\n");
    out = response_string_extend(out, rdata->body);

    return out;
}

char *response_string_extend(char *dest, const char *src) {
    dest = realloc(dest, strlen(dest) + strlen(src) + 1);

    if (dest == NULL) {
        logger_log(LOGGER_CRITICAL, "RESPONSE", "Failed to re-allocate response string");
        exit(EXIT_FAILURE);
    }

    strcat(dest, src);

    return dest;
}

void response_add_header(struct response_data *rdata, const char *name, const char *value) {
    rdata->nheaders++;

    char *name_heap  = malloc(strlen(name)  + 1);
    char *value_heap = malloc(strlen(value) + 1);

    if (name_heap == NULL || value_heap == NULL) {
        logger_log(LOGGER_CRITICAL, "RESPONSE", "Failed to allocate header pair");
        exit(EXIT_FAILURE);
    }

    strcpy(name_heap, name);
    strcpy(value_heap, value);

    rdata->header_names =  realloc(rdata->header_names,  sizeof(char *) * rdata->nheaders);
    rdata->header_values = realloc(rdata->header_values, sizeof(char *) * rdata->nheaders);

    if (rdata->header_names == NULL || rdata->header_values == NULL) {
        logger_log(LOGGER_CRITICAL, "RESPONSE", "Failed to re-allocate header arrays");
        exit(EXIT_FAILURE);
    }

    rdata->header_names[rdata->nheaders - 1] = name_heap;
    rdata->header_values[rdata->nheaders - 1] = value_heap;
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

    // Currently, the only other case we handle (404) is handled elsewhere so assume 200

    logger_log(LOGGER_INFO, "RESPONSE", "Returning 200");
    rdata->code = 200;
    status = "OK";

    rdata->status = malloc(strlen(status) + 1);
    strcpy(rdata->status, status);
}

void response_headers(struct response_data *rdata, enum content_type ctype) {
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

    if (ctype == CONTENT_HTML) {
        response_add_header(rdata, "Content-Type", "text/html; charset=utf-8");
    } else if (ctype == CONTENT_CSS) {
        response_add_header(rdata, "Content-Type", "text/css; charset=utf-8");
    } else {
        logger_log(LOGGER_ERROR, "RESPONSE", "Invalid Content type when building response, assuming html");
        response_add_header(rdata, "Content-Type", "text/html; charset=utf-8");
    }
    response_add_header(rdata, "Content-Length", length_str);
}
