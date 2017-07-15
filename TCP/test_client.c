#include "TCPConnection.h"

int main(int argc, char **argv)
{
    int sockfd;
    sockfd = create_socket(AF_INET, SOCK_STREAM /*| SOCK_NONBLOCK | SOCK_CLOEXEC*/, 0);
    if (sockfd == -1) {
    	return -1;
    }

    if (!socket_connect(sockfd, "127.0.0.1", 10001)) {
        return -1;
    }

    char buf[100];
    while(true) {
        int ret_len = read(STDIN_FILENO, buf, sizeof(buf));
        if (ret_len == 0) {
            printf("read EOF\n");
            close(sockfd);
        } else if (ret_len == -1) {
            ErrorString("read");
            close(sockfd);
            return -1;
        } else {
            // read '\n' from stdin, so here change '\n' to '\0'
            buf[ret_len - 1] = '\0';
            printf("read buf: %s\n", buf);
        }
	    ret_len = write(sockfd, buf, ret_len);
	    if (ret_len == 0) {
	    	printf("write block\n");
	    } else if (ret_len == -1) {
	    	ErrorString("write");
            // this socket will come to TIME_WAIT status
	    	close(sockfd);
	    	break;
	    }
	}

	close(sockfd);
}