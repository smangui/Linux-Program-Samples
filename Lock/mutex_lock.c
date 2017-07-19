
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h> // for malloc
#include <unistd.h> // for sysconf

#include "mutex_lock.h"
#include "utils.h"


void* create_mutex(char *lock_name)
{
	MutexLock *obj = (MutexLock *)malloc(sizeof(MutexLock));
	memset(obj, 0, sizeof(MutexLock));

	char file_name[100];
	strcpy(file_name, "/tmp/");
	strcat(file_name, lock_name);

	obj->fd = open_file(file_name, O_CREAT | O_EXCL | O_TRUNC | O_RDWR, S_IRWXU | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, sizeof(pthread_mutex_t));
	if (obj->fd == -1) {
		free(obj);
		return NULL;
	}

	obj->mMutex = (pthread_mutex_t *)mmap_file(obj->fd, sizeof(pthread_mutex_t));
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
			munmap_file(obj, sizeof(pthread_mutex_t));
		}
		pthread_mutexattr_destroy(&attr);
	}

	if (fchmod(obj->fd, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH) == -1) {
		ErrorString("create_mutex: fchmod");
		// here unlink the file will make client side cannot open success
		unlink(file_name);		
	}

	close(obj->fd);
	return obj;
}

void* open_mutex(char *lock_name)
{
	MutexLock *obj = (MutexLock *)malloc(sizeof(MutexLock));
	memset(obj, 0, sizeof(MutexLock));

	char file_name[100];
	strcpy(file_name, "/tmp/");
	strcat(file_name, lock_name);
	obj->fd = open(file_name, O_RDWR);
	if (obj->fd == -1) {
		free(obj);
		return NULL;
	}

	obj->mMutex = (pthread_mutex_t *)mmap_file(obj->fd, sizeof(pthread_mutex_t));
	if (obj->mMutex == NULL) {
		free(obj);
		return NULL;
	}

	// client side not need to call pthread_mutexattr_xxx functions

	close(obj->fd);

	return obj;
}

bool lock_mutex(void* obj)
{
	MutexLock *lock = (MutexLock *)obj;

	assert(lock->mMutex);

	if ((errno = pthread_mutex_lock(lock->mMutex)) != 0) {
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

	munmap_file(lock->mMutex, sizeof(pthread_mutex_t));
	free(obj);
}