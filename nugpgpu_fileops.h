#ifndef __NUGPGPU_FILEOPS_H__
#define __NUGPGPU_FILEOPS_H__

#include <linux/fs.h>

struct file* file_open(const char* path, int flags, int rights) ;
void file_close(struct file* file) ;
int file_read(struct file* file, unsigned long long offset, unsigned char* data, unsigned int size) ;
int file_write(struct file* file, unsigned long long offset, unsigned char* data, unsigned int size) ;
int file_sync(struct file* file) ;
void file_dump_page(struct page* page, unsigned int offset);
void file_dump_ptr(void *ptr, unsigned char *file_name, unsigned size);

#endif
