#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>

#include "QueryHandler.h"

#define MAX_LISTEN_COUNT 64
#define THREAD_POOL_SIZE 4

int main(int argc, char** argv) {

	if(getopt(argc, argv, "p:") == -1 || optarg == NULL){
		perror("Getopt error(pass port for server: -p)");
		exit(EXIT_FAILURE);
	}
	int port = atoi(&optarg[0]);

	int proxySocket, clientSocket;
	if((proxySocket = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("Socket creation error");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in serverAddress;
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if(bind(proxySocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) != 0){
		close(proxySocket);
		perror("Socket binding error");
		exit(EXIT_FAILURE);
	}
	if(listen(proxySocket, MAX_LISTEN_COUNT) != 0){
		close(proxySocket);
		perror("Listening error");
		exit(EXIT_FAILURE);
	}

	printf("Server is ready to accept connections\n");
	
	int numOfClients = 0;
	while (1) {
		struct sockaddr_in clientAddress;
		memset(&clientAddress, 0, sizeof(clientAddress));
		int clientAddressLen = 0;
		if((clientSocket = accept(proxySocket, (struct sockaddr*)&clientAddress, &clientAddressLen)) == -1){
			perror("Accepting error");
			close(proxySocket);
			exit(EXIT_FAILURE);
		}
		printf("Accepted client %s\n", inet_ntoa(clientAddress.sin_addr));

		pthread_t tid;
		if(pthread_create(&tid, NULL, handleQuery, (void*)clientSocket) != 0){
			perror("Thread creation error");
			close(proxySocket);
			close(clientSocket);
			exit(EXIT_FAILURE);
		};
		pthread_detach(tid);
		numOfClients++;
	}
	close(proxySocket);
	return EXIT_SUCCESS;
}