#include "ThreadPool.h"


int threadPoolInit(int numOfThreads){
    PoolTids = (pthread_t*)malloc(numOfThreads * sizeof(pthread_t));
    TaskQueue = queue_init(100);
    for (int i = 0; i < numOfThreads; ++i){
        if(pthread_create(&PoolTids[i], NULL, threadFunction, NULL) != 0){
            fprintf(stderr, "Thread creation error\n");
            return EXIT_FAILURE;
        }
    }
}

void* threadFunction(void* arg){
    while(1){
        int socket;
        if(queue_get(TaskQueue, &socket) == 0){
            //обработка клиента
        } else {
            fprintf(stderr, "Queue corrupted, exiting\n");
            break;
        }
    }
}

int threadPoolDestroy(){
    free(PoolTids);
    queue_destroy(TaskQueue);
}