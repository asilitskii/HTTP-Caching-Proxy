#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#define ERROR_HANDLER(msg) do{ perror(msg); exit(EXIT_FAILURE); } while(0)

#define BUFFER_SIZE 4096
#define MAX_LISTEN_COUNT 128

int main(int argc, char** argv) {
	if(getopt(argc, argv, "p:") == -1 || optarg == NULL){
		ERROR_HANDLER("Getopt error(pass port for server: -p)");
	}
	int port = atoi(&optarg[0]);

	int serverSocket, clientSocket;
	if((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		ERROR_HANDLER("Socket creation error"); 
	}

	struct sockaddr_in serverAddress;
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if(bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1){
		ERROR_HANDLER("Socket binding error");
	}
	if(listen(serverSocket, MAX_LISTEN_COUNT) == -1){
		ERROR_HANDLER("Listening error");
	}
	struct sockaddr_in clientAddress;
	memset(&clientAddress, 0, sizeof(clientAddress));
	int clientAddressLen = 0;

	while (1) {
		if((clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLen)) == -1){
			ERROR_HANDLER("Accepting error");
		}
		printf("received: from client %s\n", inet_ntoa(clientAddress.sin_addr));
		int childPid = fork();
		switch(childPid){
			case 0:
				close(serverSocket);
				char buffer[BUFFER_SIZE];
				while(1){
					int len = recvfrom(clientSocket, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&clientAddress, &clientAddressLen);
					if (len == -1){
						printf("Connection lost\n");
						break;
					}
					if(len > 0){
						//int len = read(clientSocket, buffer, sizeof(buffer));
						buffer[len] = '\0';
						printf("received: '%s' from client %s\n", buffer, inet_ntoa(clientAddress.sin_addr));
						sendto(clientSocket, buffer, len, 0, (struct sockaddr *)&clientAddress, sizeof(clientAddress));
					}
				}	
			break;
			case -1:
				ERROR_HANDLER("Forking error");
			break;
			default:
				close(clientSocket);
			break;
		}
		
	}
	close(serverSocket);
	return EXIT_SUCCESS;
}