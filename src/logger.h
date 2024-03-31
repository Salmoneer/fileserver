#ifndef SERVER_LOGGER_H
#define SERVER_LOGGER_H

enum logger_severity {
    LOGGER_INFO,
    LOGGER_WARNING,
    LOGGER_ERROR,
    LOGGER_CRITICAL
};

/**
* Log an error message.
*
* @param severity Tag to display next to message
* @param source Where the message originates from
* @param message Message to display
*/
void logger_log(enum logger_severity severity, const char *source, const char *message);

#endif // SERVER_LOGGER_H
