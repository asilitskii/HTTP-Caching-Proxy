#include "QueryHandler.h"

int resolveHost(char* query, struct sockaddr_in* hostAddr){
	char* hostNameStart = strstr(query, "Host: ");
    if (hostNameStart == NULL) 
        return -1;
    hostNameStart += 6;

    char* hostNameEnd = strchr(hostNameStart, '\r');
    if (hostNameEnd == NULL) 
        return -1;

	*hostNameEnd = '\0';
    struct hostent* host = gethostbyname(hostNameStart);
	*hostNameEnd = '\r';
    if (host == NULL) 
        return -1;

    memcpy(&hostAddr->sin_addr, host->h_addr, host->h_length);
	hostAddr->sin_family = AF_INET;
    hostAddr->sin_port = htons(80);

	return 0;
}

void* handleQuery(void* arg){
	int clientSocket = (int)arg;
	char buffer[BUFFER_SIZE];
    printf("------------------------Query processing started------------------------------\n");

	size_t bytesReceived;
    bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived < 0) {
        perror("Query receive error");
        close(clientSocket);
        pthread_exit(NULL);
    }
    else if (bytesReceived == 0){
        perror("No data received from query");
        close(clientSocket);
		pthread_exit(NULL);
    }
	else if(bytesReceived == BUFFER_SIZE){
		perror("Query is too long");
		close(clientSocket);
		pthread_exit(NULL);
	}
    
    buffer[bytesReceived] = '\0';
    printf("Query: %s", buffer);

    struct sockaddr_in destinationAddr;
	if(resolveHost(buffer, &destinationAddr) != 0){
		close(clientSocket);
		pthread_exit(NULL);
	}

	int destinationSocket;
    if ((destinationSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Destination socket creation error");
        close(clientSocket);
        pthread_exit(NULL);
    }
    if (connect(destinationSocket, (struct sockaddr*)&destinationAddr, sizeof(destinationAddr)) < 0) {
        perror("Connection to destination server failed");
        close(clientSocket);
        close(destinationSocket);
        pthread_exit(NULL);
    }

    if (send(destinationSocket, buffer, bytesReceived, 0) == -1) {
        perror("Query sending failed");
        close(clientSocket);
        close(destinationSocket);
        pthread_exit(NULL);
    }

    while (1) {
		bytesReceived = recv(destinationSocket, buffer, sizeof(buffer), 0);
		if(bytesReceived < 0){
			perror("Response receiving failed");
            close(clientSocket);
        	close(destinationSocket);
        	pthread_exit(NULL);
		}
		else if(bytesReceived == 0){
			break;
		}

        if (send(clientSocket, buffer, bytesReceived, 0) == -1) {
            perror("Response sending failed");
            close(clientSocket);
        	close(destinationSocket);
        	pthread_exit(NULL);
        }
    }

    printf("------------------------Query processing finished-----------------------------\n");

    close(clientSocket);
    close(destinationSocket);
}