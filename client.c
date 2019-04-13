#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define C2S_MAX_SIZE 128
#define S2C_MAX_SIZE 65536 + 10
#define C2SSHM "/c2sshm"
#define S2CSHM "/s2cshm"

/**
 * argv[1] times
 * argv[2] size
 */
int main(int argc, char* argv[]) {
  int times = atoi(argv[1]);
  int size = atoi(argv[2]);

  int c2sfd = shm_open(C2SSHM, O_RDWR, 0777);
  if (c2sfd < 0) {
    printf("error open %s object\n", C2SSHM);
    return 0;
  }
  /**
   * c2sbuf[0] counter
   * c2sbuf[1] size
   */
  int* c2sbuf = (int*)mmap(NULL, C2S_MAX_SIZE, PROT_READ | PROT_WRITE,
                           MAP_SHARED, c2sfd, 0);
  close(c2sfd);

  int s2cfd = shm_open(S2CSHM, O_RDWR, 0777);
  if (s2cfd < 0) {
    printf("error open %s object\n", S2CSHM);
    return 0;
  }
  char* s2cbuf =
      (char*)mmap(NULL, S2C_MAX_SIZE, PROT_READ, MAP_SHARED, s2cfd, 0);
  close(s2cfd);

  int i = 0;
  c2sbuf[0] = 0;

  struct timeval tv0, tv1;
  gettimeofday(&tv0, NULL);

  for (i; i < times; i++) {
    c2sbuf[0] = 1 - c2sbuf[0];
    c2sbuf[1] = size;
    char lasts2ccounter = s2cbuf[0];
    while (s2cbuf[0] == lasts2ccounter)
      ;
    // printf("%s\n", s2cbuf + sizeof(char));

    // sleep(1);
  }

  gettimeofday(&tv1, NULL);

  printf("time0 = %ld.%ld\ntime1 = %ld.%ld\nInterval = %ld us\n", tv0.tv_sec,
         tv0.tv_usec, tv1.tv_sec, tv1.tv_usec,
         (tv1.tv_sec - tv0.tv_sec) * 1000000 + (tv1.tv_usec - tv0.tv_usec));

  return 0;
}
