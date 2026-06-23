#ifndef THREAD
#define THREAD

#include <pthread.h>
#include <stdio.h>
#include <stdatomic.h>

pthread_t createThread(void*(*func)(void*), void *data, bool detached);

#endif
