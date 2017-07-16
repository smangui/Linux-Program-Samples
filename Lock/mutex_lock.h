#pragma once

#include <pthread.h>

typedef struct _MutexLock
{
	pthread_mutex_t *mMutex;
	int fd;
}MutexLock;

void* create_mutex(char *lock_name);
void* open_mutex(char *lock_name);
bool lock_mutex(void* obj);
bool unlock_mutex(void* obj);
bool destroy_mutex(void* obj);