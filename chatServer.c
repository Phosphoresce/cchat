#define _XOPEN_SOURCE 500
#define BUFF_SIZE 256

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

struct message {
	int cid;
	char s1[BUFF_SIZE];
	char s2[BUFF_SIZE];
};

struct user {
	int active;
	struct sockaddr_in addr;
};

/*
 * Function: main
 *   execution entrypoint
 * Input parameters:
 *   command line arguments
 * Output:
 *   returns 0 on success, 1 on failure
 */
int main(int argc, char **argv) {
	int c = 1;
	int i, j;
	int sockfd;
	int debug = 0;
	int currcid = 1;
	int recvlen;
	unsigned int port = 0;
	char hostname[BUFF_SIZE];

	struct sockaddr_in server, client;
	struct message serv_msg, client_msg;
	struct user usertable[10];
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
	
	usertable[0].active = 1;
	for (i = 1; i <= 10; i++) {
		usertable[i].active = 0;
	}

	/* connection settings */
	gethostname(hostname, BUFF_SIZE);
	memset((char *) &server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port);

	printf("%s: waiting for data on port UDP %d\n", argv[0], port);

	/* create socket */
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("Failed to allocate socket.\n");
		return 1;
	}

	/* bind */
	if (bind(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
		printf("Failed to bind to socket.\n");
		close(sockfd);
		return 1;
	}

	for (;;) {
		/* recv */
		recvlen = recvfrom(sockfd, &client_msg, sizeof(client_msg), 0, (struct sockaddr *) &client, &addrlen);
		if (recvlen > 0) {
			if (debug) {
				printf("DEBUG: Recieving <%d %s %s>\n", client_msg.cid, client_msg.s1, client_msg.s2);
			}
		} else {
			printf("cannot receive QUIT_ACK\n");
			close(sockfd);
			return 1;
		}

		/* compose */
		if(!strcmp(client_msg.s1, "JOIN")) {
			/* find unused cid */
			while (c) {
				if (usertable[currcid].active == 0) {
					usertable[currcid].addr = client;
					usertable[currcid].active = 1;
					currcid++;
					c = 0;
				} else {
					currcid++;
				}
			}
			if (currcid > 10) {
				currcid = 1;
			}
			c = 1;

			serv_msg.cid = currcid-1;
			strcpy(serv_msg.s1, client_msg.s1);
			strcpy(serv_msg.s2, hostname);

			/* send */
			if (debug) {
				printf("DEBUG: Sending <%d %s %s>\n", serv_msg.cid, serv_msg.s1, serv_msg.s2);
			}
			for (j = 1; j <= 10; j++) {
				client = usertable[j].addr;
				sendto(sockfd, &serv_msg, sizeof(serv_msg)+1, 0, (struct sockaddr *) &client, sizeof(client));
			}
		} else if (!strcmp(client_msg.s1, "QUIT")) {
			/* mark cid unused */
			serv_msg.cid = abs(client_msg.cid);
			usertable[client_msg.cid].active = 0;
			strcpy(serv_msg.s1, client_msg.s1);
			strcpy(serv_msg.s2, hostname);

			/* send */
			if (debug) {
				printf("DEBUG: Sending <%d %s %s>\n", serv_msg.cid, serv_msg.s1, serv_msg.s2);
			}
			for (j = 1; j <= 10; j++) {
				client = usertable[j].addr;
				sendto(sockfd, &serv_msg, sizeof(serv_msg)+1, 0, (struct sockaddr *) &client, sizeof(client));
			}
		} else {
			/* send to every cid but senders */
			serv_msg.cid = client_msg.cid;
			strcpy(serv_msg.s1, hostname);
			strcpy(serv_msg.s2, client_msg.s2);

			/* send */
			if (debug) {
				printf("DEBUG: Sending <%d %s %s>\n", serv_msg.cid, serv_msg.s1, serv_msg.s2);
			}
			for (j = 1; j <= 10; j++) {
				client = usertable[j].addr;
				if (j != client_msg.cid) {
					sendto(sockfd, &serv_msg, sizeof(serv_msg)+1, 0, (struct sockaddr *) &client, sizeof(client));
				}
			}
		}

	}
	close(sockfd);
	return 0;
}
