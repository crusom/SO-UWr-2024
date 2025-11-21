#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#include "include/csapp.h"

int main(void) {
  int fd = Open("a.txt",0,O_RDONLY);
  char buf[0x100] = {0};


  if (Signal(SIGCHLD, SIG_IGN) == SIG_ERR) {
    perror("signal");
    exit(EXIT_FAILURE);
  }

  pid_t pid = Fork();
  switch (pid) {
    case -1:
      perror("fork");
      exit(EXIT_FAILURE);
    case 0:
      printf("[%jd] Sleeping...\n", getpid());
      sleep(2);
      ssize_t r;
      if((r = read(fd,buf,0x10)) == -1)
        printf("[%jd] Read error! %s\n", (intmax_t) getpid(), strerror(errno));
      else {
        buf[r] = 0;
        printf("[%jd] successfuly read()! %s\n",(intmax_t) getpid(), buf);
      }
      printf("[%jd] Exiting.\n", getpid());
      break;
    default:
      Close(fd);
      printf("[%jd] Closed fd\n", getpid());
      printf("[%jd] Exiting.\n", getpid());
      break;
  }
}

