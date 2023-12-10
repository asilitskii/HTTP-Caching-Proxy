#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>

#define ERROR_HANDLER(msg) do{ perror(msg); exit(EXIT_FAILURE); } while(0)

#define BUFFER_SIZE 4096

int main(int argc, char** argv) {
	if(getopt(argc, argv, "a:") == -1 || optarg == NULL){
		ERROR_HANDLER("Getopt error(no server ip address: -a)");
	}
	char serverIP[16];
	strcpy(serverIP, &optarg[0]);
	if(getopt(argc, argv, "p:") == -1 || optarg == NULL){
		ERROR_HANDLER("Getopt error(no server port: -p)");
	}
	int serverPort = atoi(&optarg[0]);

	struct sockaddr_in serverAddress;
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	inet_pton(AF_INET, serverIP, &serverAddress.sin_addr);
	serverAddress.sin_port = htons(serverPort);

	int serverSocket;
	if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		ERROR_HANDLER("Socket creation error");
	}
	int opt_val = 1;
  	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof opt_val);


	if(connect(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1){
		ERROR_HANDLER("Server connection failed");
	}

	while(1){
		const char* message = "Hello world!";

		int sentLength = sendto(serverSocket, message, strlen(message), 0, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
		char buffer[BUFFER_SIZE];
		recvfrom(serverSocket, buffer, sentLength, 0, NULL, NULL);
		buffer[sentLength] = '\0';

		printf("recieved: '%s'\n", buffer);
		sleep(1);
	}
	close(serverSocket);
	return 0;
}