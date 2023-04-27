// https://mohsensy.github.io/programming/2019/09/25/echo-server-and-client-using-sockets-in-c.html
#include <stdio.h> // perror, printf
#include <stdlib.h> // exit, atoi
#include <unistd.h> // read, write, close
#include <arpa/inet.h> // sockaddr_in, AF_INET, SOCK_STREAM, INADDR_ANY, socket etc...
#include <string.h> // memset
#include "attest_user.h"

#define CHALLENGE_N_BYTES 64
// Max 4K bytes attest token
#define TOKEN_N_BYTES (1<<12)

void write_or_exit(int fd, char* buffer, size_t len){
	if (write(fd, buffer, len) < 0) {
		perror("write error");
		exit(6);
	}
}

int main(int argc, char const *argv[]) {

	int serverFd, clientFd;
	struct sockaddr_in server, client;
	int len;
	int port = 1234;
	char challenge_buffer[CHALLENGE_N_BYTES];
	char token[TOKEN_N_BYTES];
	char error_bad_challenge[] = "ERROR, CHALLENGE MUST BE 64 BYTE";
	char error_bad_token[] = "ERROR, FAILED TO GET ATTEST TOKEN";
	int i;

	// if (argc == 2) {
	// 	port = atoi(argv[1]);
	// }

	serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (serverFd < 0) {
		perror("Cannot create socket");
		exit(1);
	}
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);
	len = sizeof(server);
	if (bind(serverFd, (struct sockaddr *)&server, len) < 0) {
		perror("Cannot bind sokcet");
		exit(2);
	}
	if (listen(serverFd, 10) < 0) {
		perror("Listen error");
		exit(3);
	}
	while (1) {
		len = sizeof(client);
		printf("\nwaiting for clients\n");
		if ((clientFd = accept(serverFd, (struct sockaddr *)&client, &len)) < 0) {
			perror("accept error");
			exit(4);
		}
		char *client_ip = inet_ntoa(client.sin_addr);
		printf("Accepted new connection from a client %s:%d\n", client_ip, ntohs(client.sin_port));
		memset(challenge_buffer, 0, sizeof(challenge_buffer));
		int size = read(clientFd, challenge_buffer, sizeof(challenge_buffer));
		if ( size < 0 ) {
			perror("read error");
			exit(5);
		}

		// handle bad challenge length
		printf("received challenge from client, size = %d\n",size);
		if (size != 64){
			printf("bad input");
			write_or_exit(clientFd, error_bad_challenge, sizeof(error_bad_challenge));
			continue;
		}

		for(i=0;i<size;i++){
			printf("%c",challenge_buffer[i]);
		}
		printf("\n");

		// return length of token on success, otherwise return -1
		len = get_attest_token(token, challenge_buffer);
		if (len < 0){
			write_or_exit(clientFd, error_bad_token, sizeof(error_bad_token));
			continue;
		}
		write_or_exit(clientFd, token, len);

		close(clientFd);
	}
	close(serverFd);
	return 0;
}
