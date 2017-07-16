
#include <pthread.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h> // for malloc
#include <unistd.h> // for sysconf

#include "mutex_lock.h"
#include "utils.h"

int open_file(char *lock_name, int flags)
{
	char file_name[100];
	strcpy(file_name, "/tmp/");
	strcat(file_name, lock_name);

	int fd = open(file_name, flags, 0644);
	if (fd == -1) {
		ErrorString("open_file: open");
		return -1;
	}

	if (ftruncate(fd, sizeof(pthread_mutex_t)) == -1) {
		ErrorString("open_file: ftruncate");
		close(fd);
		unlink(file_name);
		return -1;
	}

	return fd;
}


void *mmap_file(int fd)
{
	void *addr = mmap(NULL, sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (addr == MAP_FAILED) {
		ErrorString("mmap_file mmap");
		close(fd);
		return NULL;
	}
	close(fd);

	return addr;
}

void munmap_file(void *obj)
{
	MutexLock *lock = (MutexLock *)obj;

	int ret = munmap(lock->mMutex, sizeof(pthread_mutex_t));
	if (ret == -1) {
		ErrorString("munmap_file munmap");
	}

	return;
}

void* create_mutex(char *lock_name)
{
	MutexLock *obj = (MutexLock *)malloc(sizeof(MutexLock));
	memset(obj, 0, sizeof(MutexLock));

	obj->fd = open_file(lock_name, O_CREAT | O_EXCL | O_TRUNC | O_RDWR);
	if (obj->fd == -1) {
		free(obj);
		return NULL;
	}

	obj->mMutex = (pthread_mutex_t *)mmap_file(obj->fd);
	if (obj->mMutex == NULL) {
		free(obj);
		return NULL;
	} else {
		pthread_mutexattr_t attr;
		pthread_mutexattr_init(&attr);
		pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
		pthread_mutexattr_setrobust_np(&attr, PTHREAD_MUTEX_ROBUST_NP);
		if (pthread_mutex_init(obj->mMutex, &attr) != 0 ) {
			ErrorString("create_mutex: pthread_mutex_init");
			munmap_file(obj);
		}
		pthread_mutexattr_destroy(&attr);
	}

	return obj;
}

void* open_mutex(char *lock_name)
{
	MutexLock *obj = (MutexLock *)malloc(sizeof(MutexLock));
	memset(obj, 0, sizeof(MutexLock));

	obj->fd = open_file(lock_name, O_RDWR);
	if (obj->fd == -1) {
		free(obj);
		return NULL;
	}

	obj->mMutex = (pthread_mutex_t *)mmap_file(obj->fd);
	if (obj->mMutex == NULL) {
		free(obj);
		return NULL;
	} else {
		/*
		pthread_mutexattr_t attr;
		pthread_mutexattr_init(&attr);
		pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
		if (pthread_mutex_init(obj->mMutex, &attr) != 0 ) {
			ErrorString("create_mutex: pthread_mutex_init");
			munmap_file(obj);
		}
		pthread_mutexattr_destroy(&attr);
		*/
	}

	return obj;
}

bool lock_mutex(void* obj)
{
	MutexLock *lock = (MutexLock *)obj;

	assert(lock->mMutex);

	if ((errno = pthread_mutex_unlock(lock->mMutex)) != 0) {
		ErrorString("lock_mutex: pthread_mutex_lock");
		return false;
	}

	return true;
}

bool unlock_mutex(void* obj)
{
	MutexLock *lock = (MutexLock *)obj;

	assert(lock->mMutex);

	if ((errno = pthread_mutex_unlock(lock->mMutex)) != 0) {
		ErrorString("unlock_mutex: pthread_mutex_unlock");
		return false;
	}

	return true;
}

bool destroy_mutex(void* obj)
{
	MutexLock *lock = (MutexLock *)obj;

	assert(lock->mMutex);

	if (pthread_mutex_destroy(lock->mMutex) != 0) {
		ErrorString("destroy_mutex: pthread_mutex_destroy");
		return false;	
	}

	munmap_file(obj);
	free(obj);
}