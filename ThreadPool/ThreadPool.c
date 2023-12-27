#include "ThreadPool.h"

int initThreadPool(int numOfThreads){
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
        task_t task;
        int code = queue_get(TaskQueue, &task);
        if(code == 0){
            handleClient(arg);
        } else if(code == EAGAIN){
            sleep(1);
        }
         else {
            fprintf(stderr, "Queue corrupted, exiting\n");
            break;
        }
    }
}

int addTask(task_t task){
    queue_add(TaskQueue, task);
}

int threadPoolDestroy(){
    free(PoolTids);
    queue_destroy(TaskQueue);
}