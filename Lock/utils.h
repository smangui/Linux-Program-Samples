void ErrorString(const char *str);

int open_file(char *lock_name, int flags, int mode, size_t size);
void *mmap_file(int fd, size_t length);
void munmap_file(void *obj, size_t length);