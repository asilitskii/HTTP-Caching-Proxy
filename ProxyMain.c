#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <pthread.h>

#include "ThreadPool.h"

#define ERROR_HANDLER(msg) do{ perror(msg); exit(EXIT_FAILURE); } while(0)

#define BUFFER_SIZE 4096
#define MAX_LISTEN_COUNT 128
#define THREAD_POOL_SIZE 4

typedef struct HandlerArgs{
	int clientSocket;
	struct sockaddr_in clientAddress;
	int clientAddressLen;
} HandlerArgs_t;

void* clientHandler(void* arg){
	//HandlerArgs_t* args = (HandlerArgs_t*)arg; 
	int clientSocket = ((HandlerArgs_t*)arg)->clientSocket;
	struct sockaddr_in clientAddress = ((HandlerArgs_t*)arg)-> clientAddress;
	int clientAddressLen = ((HandlerArgs_t*)arg)->clientAddressLen;
	//fprintf(stderr, "Socket: %d\n", clientSocket);
	char buffer[BUFFER_SIZE];
	while(1){
		int len = recvfrom(clientSocket, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&clientAddress, &clientAddressLen);
		if (len == -1){
			fprintf(stderr, "Connection lost\n");
			close(clientSocket);
			break;
		}
		if(len > 0){
			//int len = read(clientSocket, buffer, sizeof(buffer));
			buffer[len] = '\0';
			printf("Received: '%s' from client %s:%d\n", buffer, inet_ntoa(clientAddress.sin_addr), clientAddress.sin_port);
			sendto(clientSocket, buffer, len, 0, (struct sockaddr *)&clientAddress, sizeof(clientAddress));
		}
	}	
}

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

	int numOfClients = 0;

	while (1) {
		if((clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLen)) == -1){
			ERROR_HANDLER("Accepting error");
		}
		HandlerArgs_t* handlerArgs = malloc(sizeof(HandlerArgs_t));
		handlerArgs->clientSocket = clientSocket;
		handlerArgs->clientAddress = clientAddress;
		handlerArgs->clientAddressLen = clientAddressLen;
		printf("Accepted client %s\n", inet_ntoa(handlerArgs->clientAddress.sin_addr));
		pthread_t tid;
		if(pthread_create(&tid, NULL, clientHandler, (void*)handlerArgs) != 0){
			ERROR_HANDLER("Thread creation error");
		};
		
	}
	close(serverSocket);
	return EXIT_SUCCESS;
}