#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "queue.h"



static pthread_t* PoolTids;

static queue_t* TaskQueue;

int initThreadPool(int numOfThreads);

void* threadFunction(void* arg);