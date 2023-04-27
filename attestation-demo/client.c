// https://mohsensy.github.io/programming/2019/09/25/echo-server-and-client-using-sockets-in-c.html
#include <stdio.h> // perror, printf
#include <stdlib.h> // exit, atoi
#include <unistd.h> // write, read, close
#include <arpa/inet.h> // sockaddr_in, AF_INET, SOCK_STREAM, INADDR_ANY, socket etc...
#include <string.h> // strlen, memset
#include "common.h"
#include "parser.h"
#define CHALLENGE_N_BYTES 64
#define CHALLENGE_DEFAULT_BYTE  0x42

void print_challenge(char*buf, size_t buf_len){
	int i;
	for (i=0;i<buf_len;i++){
		printf("%c",buf[i]);
	}
}

void init_challenge(int argc, char const *argv[], char*buf, size_t buf_len){
	int c;
	int i = 0;
	memset(buf, 0, CHALLENGE_N_BYTES);
	if(argc != 2) goto default_challenge;
	if(!(argv[1][0]=='-' && argv[1][1]=='c')) goto default_challenge;
	if(CHALLENGE_N_BYTES > buf_len) {
		printf("buffer to small, think twice!\n");
		exit(1);
	}
	// read challenge from stdio
    while ((c = getchar()) != EOF) {
		buf[i++] = c;
    }
	// ignore EOF
	i --;
	if (i!=CHALLENGE_N_BYTES){
		printf("read str len %d != %d, challenge 0 padded or chopped\n", i, CHALLENGE_N_BYTES);
		return;
	}

default_challenge:
	memset(buf, CHALLENGE_DEFAULT_BYTE, CHALLENGE_N_BYTES);
}


int main(int argc, char const *argv[]) {
	int serverFd;
	struct sockaddr_in server;
	int len;
	// server_ip and port hardcoded to localhost, no
	// need for paramater...
	int port = 1234;
	char *server_ip = "127.0.0.1";
	char challenge[CHALLENGE_N_BYTES];
	char recv[SZ_4K];
	int size;
	
	// init CHALLENGE
	init_challenge(argc, argv, challenge, CHALLENGE_N_BYTES);
	printf("using challenge:\n");
	print_challenge(challenge, CHALLENGE_N_BYTES);

	serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (serverFd < 0) {
		perror("Cannot create socket");
		exit(1);
	}
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(server_ip);
	server.sin_port = htons(port);
	len = sizeof(server);
	if (connect(serverFd, (struct sockaddr *)&server, len) < 0) {
		perror("Cannot connect to server");
		exit(2);
	}
	

	if (write(serverFd, challenge, CHALLENGE_N_BYTES) < 0) {
		perror("Cannot write");
		exit(3);
	}

	memset(recv, 0, sizeof(recv));
	size = read(serverFd, recv, sizeof(recv));
	if (size < 0) {
		perror("cannot read");
		exit(4);
	}
	// sanity check:
	if (size < 128){
		printf("sanity check failed..%s\n",recv);
		return 0;

	}

	printf("Received token from server, len=%d\n", size);
	printf("trying to parse the token: \n");
	
	test_parse(recv, size);
	// int i;
	// for(i=0;i<size;i++){
	// 	printf("0x%x ", recv[i]);
	// }


	close(serverFd);
	return 0;
}
