#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

/**
* Initialise the server on `port`.
* @param port Port to recieve connections on
* @return Socket the server is running on
*/
int server_init(int port);

/**
* Read data sent to `socket`
* @param sockfd Socket that has accepted a connection
* @return Data sent to socket
*/
char *server_read(int sockfd);

#endif  // SERVER_SERVER_H
