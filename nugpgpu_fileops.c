#include "nugpgpu_fileops.h"

#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/buffer_head.h>

/* 
 * Referenced from the stackoverflow link mentioned below.
 * Credits to http://stackoverflow.com/questions/1184274/how-to-read-write-files-within-a-linux-kernel-module
 */

struct file* file_open(const char* path, int flags, int rights) {
    struct file* filp = NULL;
    mm_segment_t oldfs;
    int err = 0;

    oldfs = get_fs();
    set_fs(get_ds());
    filp = filp_open(path, flags, rights);
    set_fs(oldfs);
    if(IS_ERR(filp)) {
        err = PTR_ERR(filp);
        return NULL;
    }
    return filp;
}

void file_close(struct file* file) {
    filp_close(file, NULL);
}

int file_read(struct file* file, unsigned long long offset, unsigned char* data, unsigned int size) {
    mm_segment_t oldfs;
    int ret;

    oldfs = get_fs();
    set_fs(get_ds());
    ret = vfs_read(file, data, size, &offset);

    set_fs(oldfs);
    return ret;
}

int file_write(struct file* file, unsigned long long offset, unsigned char* data, unsigned int size) {
    mm_segment_t oldfs;
    int ret;

    oldfs = get_fs();
    set_fs(get_ds());

    ret = vfs_write(file, data, size, &offset);

    set_fs(oldfs);
    return ret;
}

int file_sync(struct file* file) {
    vfs_fsync(file, 0);
    return 0;
}

void file_dump_page(struct page *page, unsigned int offset)
{
  #define dump_file_name "/tmp/page_"
  unsigned char file_name[100];
  unsigned char page_num[100];

  sprintf(page_num, "%u", offset);
  strcpy(file_name, dump_file_name);
  strcat(file_name, page_num);

  file_dump_ptr(kmap(page), file_name, PAGE_SIZE);
  #undef dump_file_name
  return;
}


void file_dump_ptr(void *ptr, unsigned char *file_name, unsigned size)
{
  struct file* flip;
  flip = file_open(file_name, O_CREAT | O_RDWR | O_TRUNC, 0);

  file_write(flip, 0, ptr, size);
  file_sync(flip);
  file_close(flip);
}

