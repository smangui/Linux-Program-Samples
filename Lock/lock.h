#pragma once

#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <stdbool.h> 

#include "mutex_lock.h"


// Should be careful that here use ','
typedef enum _LOCK_TYPE
{
	UNKNOWN_TYPE = 0,
	MUTEX_LOCK,
	SEM_LOCK,
	FILE_LOCK,
}LOCK_TYPE;


// Should be careful that here use ';'
typedef struct __lock_functions_table
{
	void* (*create_lock) (char *lock_name);
	void* (*open_lock) (char *lock_name);
	bool (*lock) (void* obj);
	bool (*unlock) (void* obj);
	bool (*destroy_lock) (void* obj);
}lock_functions_table;


// Should be careful that here use ','
static lock_functions_table mutex_functions_table =
{
	create_mutex,
	open_mutex,
	lock_mutex,
	unlock_mutex,
	destroy_mutex,
};


class Lock
{
public:
	Lock(LOCK_TYPE type, const char *lock_name);
	~Lock();

	bool create_lock();
	bool open_lock();
	bool lock();
	bool unlock();
	bool destroy_lock();
private:
	LOCK_TYPE mType;
	char *mLockName;
	void *mLockObj;
	bool mInit;
	lock_functions_table *mFuncs;
};

