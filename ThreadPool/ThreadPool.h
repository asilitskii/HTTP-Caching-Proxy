#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "ClientHandler.h"
#include "queue.h"
#include "Task.h"

static pthread_t* PoolTids;

static queue_t* TaskQueue;

int initThreadPool(int numOfThreads);

void* threadFunction(void* arg);

int addTask(task_t task);