#pragma once 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

typedef struct _QueueNode {
	int value;
	struct _QueueNode *next;
} qnode_t;

typedef struct _Queue {
	qnode_t *first;
	qnode_t *last;
	int size;
	int maxCapacity;
} queue_t;

queue_t* queue_init(int maxCapacity);
void queue_destroy(queue_t *q);
int queue_add(queue_t *q, int value);
int queue_get(queue_t *q, int* value);
