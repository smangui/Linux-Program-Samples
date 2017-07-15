#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>    // for bool, true, false
#include <errno.h>      // for errno
#include <string.h>     // for strerror
#include <netinet/in.h> // for sockaddr_in
#include <netinet/tcp.h> // for sockaddr_in, TCP_XXX defined

typedef union __sock_optval {
   int i_val;
   long l_val;
   struct linger linger_val;
   struct timeval timeval_val;
} sock_optval;


void ErrorString(const char *function);


int create_socket(int domain, int type, int protocol);
bool socket_bind(int sockfd, char *ip, int port);
bool socket_listen(int sockfd, int backlog);
int socket_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
bool socket_connect(int sockfd, const char *ip, int port);
bool socket_set_not_delay(int sockfd);
bool socket_set_reuseaddr(int sockfd);
/* This is available since kernel 3.9 
bool socket_set_reuseport(int sockfd);
*/
bool socket_set_keep_alive(int sockfd);
bool socket_set_cork(int sockfd, int val);
bool socket_set_linger(int sockfd, int l_onoff, int l_linger);
bool socket_get_sockopt(int sockfd, int optname);