
#include <assert.h>
#include <string.h> // for strxxx
#include <stdlib.h> // for free()

#include "lock.h"

Lock::Lock(LOCK_TYPE type, const char *lock_name)
	: mType(type),
	  mLockName(NULL),
	  mLockObj(NULL),
	  mFuncs(NULL),
	  mInit(false)
{
	switch (mType) {
		case MUTEX_LOCK:
			mFuncs = &mutex_functions_table;
			break;
		default:
			goto failed;
			break;
	}
	mLockName = new char[strlen(lock_name) + 1];
	strcpy(mLockName, lock_name);
	mInit = true;
	return;
failed:
	mInit = false;
	return;
}


Lock::~Lock()
{
	if (mLockObj && mFuncs) {
		mFuncs->destroy_lock(mLockObj);
		mLockObj == NULL;
	}
	if (mLockName) {
		free(mLockName);
	}
	mInit = false;
}


bool
Lock::create_lock()
{
	assert(mInit == true);

	mLockObj = mFuncs->create_lock(mLockName);
	if (!mLockObj) {
		return false;
	}
	return true;
}

bool
Lock::open_lock()
{
	assert(mInit == true);

	mLockObj = mFuncs->open_lock(mLockName);
	if (!mLockObj) {
		return false;
	}
	return true;
}

bool
Lock::lock()
{
	assert(mInit == true);

	mFuncs->lock(mLockObj);

	return true;
}

bool Lock::unlock()
{
	assert(mInit == true);

	mFuncs->unlock(mLockObj);

	return true;
}

bool Lock::destroy_lock()
{
	assert(mInit == true);

	mFuncs->destroy_lock(mLockObj);

	return true;
}