#include "TCPConnection.h"

int main(int argc, char **argv)
{
    int sockfd;
    sockfd = create_socket(AF_INET, SOCK_STREAM /*| SOCK_NONBLOCK | SOCK_CLOEXEC*/, 0);
    if (sockfd == -1) {
    	return -1;
    }

    if (!socket_bind(sockfd, "127.0.0.1", 10001)) {
    	return -1;
    }

    if (!socket_listen(sockfd, 5)) {
    	return -1;
    }

    struct sockaddr addrCli;
    int sockCli, socklen = sizeof(addrCli);
    if ((sockCli = socket_accept(sockfd, &addrCli, &socklen)) == -1) {
    	return -1;
    }

    char buf[100];
    while(true) {
	    int read_byte = read(sockCli, buf, sizeof(buf));
	    if (read_byte == 0) {
	    	printf("receive EOF\n");
	    	close(sockCli);
	    	break;
	    } else if (read_byte == -1) {
	    	ErrorString("read");
	    	close(sockCli);
	    	break;
	    } else {
	    	printf("receive buf: %s\n", buf);
	    }
	}

	close(sockfd);
}