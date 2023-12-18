#include "queue.h"

static pthread_mutex_t mutex;

queue_t* queue_init(int maxCapacity) {
	int err;

	queue_t *queue = malloc(sizeof(queue_t));
	if (!queue) {
		printf("Cannot allocate memory for a queue\n");
		return NULL;
	}

	queue->first = NULL;
	queue->last = NULL;
	queue->maxCapacity = maxCapacity;
	queue->size = 0;

	return queue;
}

void queue_destroy(queue_t *queue) {
	if(queue->first == NULL){
		free(queue);
		return;
	}
    qnode_t *currentNode = queue->first; 
	qnode_t *nextNode = currentNode->next;
    do{
        nextNode = currentNode->next;
        free(currentNode);
        currentNode = nextNode;
    } while(nextNode != NULL);

    free(queue);
}

int queue_add(queue_t *queue, int value) {
	qnode_t *newNode = malloc(sizeof(qnode_t));
	if (newNode == NULL) {
		printf("Cannot allocate memory for new node\n");
		return -1;
	}

	if(pthread_mutex_lock(&mutex) != 0){
		fprintf(stderr, "pthread_mutex_lock() failed\n");
		free(newNode);
		return -1;
	}

	if (queue->size == queue->maxCapacity){
		free(newNode);
		if(pthread_mutex_unlock(&mutex) != 0){
			fprintf(stderr, "pthread_mutex_unlock() failed\n");
			return -1;
		}
		return 0;
	}

	newNode->value = value;
	newNode->next = NULL;

	if (!queue->first)
		queue->first = queue->last = newNode;
	else {
		queue->last->next = newNode;
		queue->last = queue->last->next;
	}

	queue->size++;

	if(pthread_mutex_unlock(&mutex) != 0){
		fprintf(stderr, "pthread_mutex_unlock() failed\n");
		return -1;
	}

	return 0;
}

int queue_get(queue_t* queue, int* value) {
	if(pthread_mutex_lock(&mutex) != 0){
		fprintf(stderr, "pthread_mutex_lock() failed\n");
		return -1;
	}

	if (queue->size == 0){
		if(pthread_mutex_unlock(&mutex) != 0){
			fprintf(stderr, "pthread_mutex_unlock() failed\n");
			return -1;
		}
		return 0;
	}
	qnode_t *tmp = queue->first;

	*value = tmp->value;
	queue->first = queue->first->next;
	queue->size--;
	if(pthread_mutex_unlock(&mutex) != 0){
		fprintf(stderr, "pthread_mutex_unlock() failed\n");
		return -1;
	}
	free(tmp);


	return 0;
}