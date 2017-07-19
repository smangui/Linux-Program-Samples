
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>      // for errno
#include <string.h>     // for strerror
#include <stdio.h>
#include "utils.h"

void ErrorString(const char *str)
{
   printf("%s failed: errno %d: %s\n", str, errno, strerror(errno));
}


int open_file(char *file_name, int flags, int mode, size_t size)
{
	int fd = open(file_name, flags, mode);
	if (fd == -1) {
		ErrorString("open_file: open");
		return -1;
	}

	void *buf = malloc(size);
	if (!buf) {
		ErrorString("malloc fail");
		close(fd);
		return -1;
	}

	memset(buf, 0, size);

	if (write(fd, buf, size) != size) {
		free(buf);
		close(fd);
		return -1;
	}

	free(buf);
	return fd;
}


void *mmap_file(int fd, size_t length)
{
	void *addr = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (addr == MAP_FAILED) {
		ErrorString("mmap_file mmap");
		close(fd);
		return NULL;
	}
	//close(fd);

	return addr;
}

void munmap_file(void *obj, size_t length)
{
	int ret = munmap(obj, length);
	if (ret == -1) {
		ErrorString("munmap_file munmap");
	}

	return;
}