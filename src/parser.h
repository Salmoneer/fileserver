#ifndef SERVER_PARSER_H
#define SERVER_PARSER_H

#include <stdlib.h>

struct parser {
    const char *data;
    size_t index;
};

enum request_method {
    GET,
    HEAD,
    POST,
    PUT,
    DELETE,
    CONNECT,
    OPTIONS,
    TRACE,
    PATCH
};

struct parser_data {
    enum request_method method;
    char *path;
    char *protocol;

    int nheaders;
    char **header_names;
    char **header_values;
};

/**
* Parse a request string into parser_data.
* @param data HTTP Request data
* @return `data` parsed into a parser_data struct
*/
struct parser_data *parser_parse(const char *data);

/**
* Deallocate the parser_data in `pdata`.
* @param pdata `parser_data` struct to free
*/
void parser_free_data(struct parser_data *pdata);

void parser_consume(struct parser *p);
char *parser_until(struct parser *p, char c);

enum request_method parser_method(struct parser *p);
void parser_headers(struct parser *p, struct parser_data *pdata);

#endif // SERVER_PARSER_H
