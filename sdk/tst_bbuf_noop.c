#include "nugpgpu.h"

int main(int argc, char **argv) {
  int fd;
  long ioresult;

  fd = NUGPGPU_OPEN_FD;
  printf("fd : %d\n", fd);

  struct nuioctl mynu;
  mynu.length = 4321;

  ioresult = ioctl(fd, NUIOCTL_EXM_NEW, &mynu);
  printf("ioresult : %ld\n", ioresult);

 return 0;
}
