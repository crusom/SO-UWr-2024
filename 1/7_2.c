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
  int fd = open("a.txt",O_RDONLY);
  char buf[0x100] = {0};


  if (signal(SIGCHLD, SIG_IGN) == SIG_ERR) {
    perror("signal");
    exit(EXIT_FAILURE);
  }

  pid_t pid = Fork();
  switch (pid) {
    case -1:
      perror("fork");
      exit(EXIT_FAILURE);
    case 0:
      off_t position = lseek(fd, 0, SEEK_CUR);
      printf("[%jd] offset position %jd\n", getpid(), position);
      printf("[%jd] Sleeping\n", getpid());
      sleep(2);
      position = lseek(fd, 0, SEEK_CUR);
      printf("[%jd] offset position %jd\n", getpid(), position);
      printf("[%jd] Exiting.\n", getpid());
      break;
    default:
      printf("[%jd] Sleeping\n", getpid());
      sleep(1);
      printf("[%jd] Read file\n", getpid());
      Read(fd,buf,0x10);
      printf("[%jd] Sleeping\n", getpid());
      sleep(2);
      printf("[%jd] Exiting.\n", getpid());
      break;
  }
}

