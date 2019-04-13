#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define C2S_MAX_SIZE 128
#define S2C_MAX_SIZE 65536 + 10
#define C2SSHM "/c2sshm"
#define S2CSHM "/s2cshm"

void read_chardev(char* buffer, int size) { syscall(392, buffer, size); }

int main() {
  int c2sfd = shm_open(C2SSHM, O_CREAT | O_RDWR | O_EXCL, 0777);

  if (c2sfd < 0) {
    c2sfd = shm_open(C2SSHM, O_RDWR, 0777);
    if (c2sfd < 0) {
      printf("error open %s object\n", C2SSHM);
      return 0;
    }
    ftruncate(c2sfd, C2S_MAX_SIZE);
  }
  int* c2sbuf = (int*)mmap(NULL, C2S_MAX_SIZE, PROT_READ, MAP_SHARED, c2sfd, 0);
  close(c2sfd);

  int s2cfd = shm_open(S2CSHM, O_CREAT | O_RDWR | O_EXCL, 0777);
  if (s2cfd < 0) {
    s2cfd = shm_open(S2CSHM, O_RDWR, 0777);
    if (s2cfd < 0) {
      printf("error open %s object\n", S2CSHM);
      return 0;
    }
    ftruncate(s2cfd, S2C_MAX_SIZE);
  }
  char* s2cbuf = (char*)mmap(NULL, S2C_MAX_SIZE, PROT_READ | PROT_WRITE,
                             MAP_SHARED, s2cfd, 0);
  close(s2cfd);

  int lastc2scounter = c2sbuf[0];
  s2cbuf[0] = 0;
  while (1) {
    if (c2sbuf[0] == lastc2scounter) {
      // printf("No request.\n");
    } else {
      // printf("Request detected, counter is %d\n", c2sbuf[0]);
      int buffer_size = c2sbuf[1];
      read_chardev(s2cbuf + sizeof(char), buffer_size);
      s2cbuf[0] = 1 - s2cbuf[0];
      lastc2scounter = c2sbuf[0];
    }

    // sleep(1);
  }

  shm_unlink(C2SSHM);
  shm_unlink(S2CSHM);

  return 0;
}
