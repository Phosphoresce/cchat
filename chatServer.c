#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#define BUFF_SIZE 255

/*
 * Function: main
 *   execution entrypoint
 * Input parameters:
 *   command line arguments
 * Output:
 *   returns 0 on success, 1 on failure
 */
int main(int argc, char **argv) {
	int sockfd;
	int debug = 0;
	int recvlen;
	unsigned int port = 0;
	char buffer[BUFF_SIZE];

	struct sockaddr_in server;
	struct sockaddr_in client;
	socklen_t addrlen = sizeof(client);

	/* runtime args */
	if (argc < 2) {
		printf("chatServer <port> <debug flag: 0 1>\n");
		return 1;
	}
	if (argc > 2) {
		port = atoi(argv[1]);
		debug = atoi(argv[2]);
	}

	/* connection settings */
	memset((char *) &server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port);

	/* create socket */
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("Failed to allocate socket.\n");
		return 1;
	}

	/* bind */
	if (bind(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
		printf("Failed to bind to socket.\n");
		return 1;
	}

	/* recv */
	recvlen = recvfrom(sockfd, buffer, BUFF_SIZE, 0, (struct sockaddr *)&client, &addrlen);
	if (recvlen > 0) {
		buffer[recvlen] = 0;
		if (debug) {
			printf("client: %s\n", buffer);
		}
	}
	sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&client, addrlen);
	return 0;
}
