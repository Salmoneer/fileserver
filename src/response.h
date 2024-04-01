#ifndef SERVER_RESPONSE_H
#define SERVER_RESPONSE_H

#include "parser.h"

struct response_data {
    char *protocol;
    int code;
    char *status;

    int nheaders;
    char **header_names;
    char **header_values;

    char *body;
};

/**
* Build a new HTTP response from a recieved request.
* @param data String recieved from client request
* @return `data` parsed into a response
*/
struct response_data *response_new(const char *data);

/**
* De-allocate the response_data in `rdata`.
* @param rdata `response_data` struct to free
*/
void response_free(struct response_data *rdata);

void response_add_header(struct response_data *rdata, const char *name, const char *value);

void response_protocol(struct parser_data *pdata, struct response_data *rdata);
void response_status(struct response_data *rdata);
void response_headers(struct response_data *rdata);

#endif // SERVER_RESPONSE_H
