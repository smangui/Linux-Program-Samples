#include "lock.h"
#include <unistd.h>
#include <stdio.h>

int main()
{
	Lock test_lock(MUTEX_LOCK, "test1");

	test_lock.create_lock();

	test_lock.lock();

	printf("get lock\n");

	sleep(10);

	test_lock.unlock();

	printf("relase lock\n");

	pause();

	//test_lock.destroy_lock();

	return 1;
}