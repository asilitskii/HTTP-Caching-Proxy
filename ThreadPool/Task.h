#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>

typedef struct HandlerArgs{
	int clientSocket;
	struct sockaddr_in clientAddress;
	int clientAddressLen;
} HandlerArgs_t;

typedef struct task_struct_t {
    int socket;
    HandlerArgs_t* args;
} task_t;