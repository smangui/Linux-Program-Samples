#include "TCPConnection.h"


void ErrorString(const char *function)
{
   printf("%s failed: errno %d: %s\n", function, errno, strerror(errno));
}


int create_socket(int domain, int type, int protocol)
{
   int sockSrv = socket(domain, type, protocol);
   if (sockSrv == -1) {
      ErrorString(__FUNCTION__);
      return -1;
   }

   return sockSrv;
}


bool socket_bind(int sockfd, char *ip, int port)
{
   struct sockaddr_in sockServer;
   int len = sizeof(sockServer);

   memset(&sockServer, 0, len);
   sockServer.sin_family = AF_INET;
   inet_pton(AF_INET, ip, &(sockServer.sin_addr.s_addr));
   sockServer.sin_port = htons(port);

   int ret = bind(sockfd, (struct sockaddr*)&sockServer, len);
   if (ret == -1) {
      ErrorString(__FUNCTION__);
      return false;
   }
   
   return true;
}


bool socket_listen(int sockfd, int backlog)
{
   int ret = listen(sockfd, backlog);
   if (ret == -1) {
      ErrorString(__FUNCTION__);
      return false;
   }

   return true;
}


int socket_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
   int ret = accept(sockfd, addr, addrlen);
   if (ret == -1) {
      ErrorString(__FUNCTION__);
      return ret;
   }

   return ret;
}


bool socket_connect(int sockfd, const char *ip, int port)
{
   struct sockaddr_in sockServer;
   int len = sizeof(sockServer);

   memset(&sockServer, 0, len);
   sockServer.sin_family = AF_INET;
   inet_pton(AF_INET, ip, &(sockServer.sin_addr.s_addr));
   sockServer.sin_port = htons(port);

   int ret = connect(sockfd, (struct sockaddr *)&sockServer, len);
   if (ret == -1) {
      ErrorString(__FUNCTION__);
      return false;
   }

   return true;
}


bool socket_set_not_delay(int sockfd)
{
   sock_optval optval;
   optval.i_val = 1;
   int ret = setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (void *)&optval, sizeof(int));
   if (ret == -1) {
      ErrorString(__FUNCTION__);
      return false;
   }

   printf("%s - set TCP_NODELAY success\n", __FUNCTION__);

   return true;
}


bool socket_set_reuseaddr(int sockfd)
{
   sock_optval optval;
   optval.i_val = 1;
   int ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (void *)&optval, sizeof(int));
   if (ret == -1) {
      ErrorString(__FUNCTION__);
      return false;
   }

   printf("%s - set SO_REUSEADDR success\n", __FUNCTION__);

   return true;
}

/* This is available since kernel 3.9
bool socket_set_reuseport(int sockfd)
{
   sock_optval optval;
   optval.i_val = 1;
   int ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, (void *)&optval, sizeof(int));
   if (ret == -1) {
      ErrorString(__FUNCTION__);
      return false;
   }

   printf("%s - set SO_REUSEPORT success\n", __FUNCTION__);

   return true;
}
*/

bool socket_set_keep_alive(int sockfd)
{
   sock_optval optval;
   optval.i_val = 1;
   int ret = setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, (void *)&optval, sizeof(int));
   if (ret == -1) {
      ErrorString(__FUNCTION__);
      return false;
   }

   printf("%s - set SO_KEEPALIVE success\n", __FUNCTION__);

   return true;
}


bool socket_set_cork(int sockfd, int val)
{
   sock_optval optval;
   optval.i_val = val;
   int ret = setsockopt(sockfd, IPPROTO_TCP, TCP_CORK, (void *)&optval, sizeof(int));
   if (ret == -1) {
      ErrorString(__FUNCTION__);
      return false;
   }

   printf("%s - set TCP_CORK %d success\n", __FUNCTION__, val);

   return true;
}


bool socket_set_linger(int sockfd, int l_onoff, int l_linger)
{
   sock_optval optval;
   optval.linger_val.l_onoff = l_onoff;
   optval.linger_val.l_linger = l_linger;
   int ret = setsockopt(sockfd, SOL_SOCKET, SO_LINGER, (void *)&optval, sizeof(struct linger));
   if (ret == -1) {
      ErrorString(__FUNCTION__);
      return false;
   }

   printf("%s - set SO_LINGER success\n", __FUNCTION__);

   return true;
}


bool socket_get_sockopt(int sockfd, int optname)
{
   sock_optval optval;
   int ret;
   int len = sizeof(optval);

   switch (optname) {
      case TCP_CORK:
         ret = getsockopt(sockfd, IPPROTO_TCP, TCP_CORK, (void *)&optval, &len);
         if (ret == -1) goto failed;
         else printf("%s - TCP_CORK default = %d\n", __FUNCTION__, optval.i_val);
         break;
      case TCP_NODELAY:
         ret = getsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (void *)&optval, &len);
         if (ret == -1) goto failed;
         else printf("%s - TCP_NODELAY default = %d\n", __FUNCTION__, optval.i_val);
         break;
      default:
         break;
   }

failed:
   if (ret == -1) {
      ErrorString(__FUNCTION__);
      return false;
   }

   return true;
}