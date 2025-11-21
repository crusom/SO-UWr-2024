#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <errno.h>

#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <stdint.h>

#include "include/csapp.h"

#define CLONE_FILES 0x00000400

void monitor_clone(pid_t child) {
  int status;
  struct user_regs_struct regs;

  while (1) {
    wait(&status);
    // did child exit?
    if (WIFEXITED(status)) break;
    ptrace(PTRACE_GETREGS, child, 0, &regs);

    if (regs.orig_rax == SYS_clone) {
      if (!(regs.rdi & CLONE_FILES)) {
        printf("Dodajemy CLONE_FILES do flag\n");

        regs.rdi |= CLONE_FILES;
        ptrace(PTRACE_SETREGS, child, 0, &regs);
      }
    }
    // continue the process
    ptrace(PTRACE_SYSCALL, child, 0, 0);
  }
}

int main() {
  pid_t child;
  // automatically clean after the child, after it dies
  if (signal(SIGCHLD, SIG_IGN) == SIG_ERR) {
    perror("signal");
    exit(EXIT_FAILURE);
  }

  child = Fork();
  if (child == 0) {
    long ret = ptrace(PTRACE_TRACEME, 0, 0, 0);
    if (ret == -1) {
      fprintf(stderr, "ptrace(PTRACE_TRACEM) error: %s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }
    // stop ourself and wait for the parent (debugger)
    kill(getpid(), SIGSTOP); 
    
    int fd = Open("a.txt",0,O_RDONLY);
    char buf[0x100] = {0};
    
    pid_t pid = Fork();
    if (pid == 0) {
      printf("[%jd] Sleeping...\n", getpid());
      sleep(1);
      ssize_t r;
      printf("[%jd] Trying to read()\n", getpid());
      if((r = read(fd,buf,0x10)) == -1) {
        printf("[%jd] Error in read()! %s\n",(intmax_t) getpid(), strerror(errno));  
      } else {
        buf[r] = 0;
        printf("[%jd] Successful read()! %s\n",(intmax_t) getpid(), buf);  
      }
      printf("[%jd] Exiting.\n", getpid());
      exit(EXIT_SUCCESS);
    } else {
      Close(fd);
      printf("[%jd] closed\n", getpid());
      printf("[%jd] Sleeping...\n", getpid());
      sleep(2);
      printf("[%jd] Exiting.\n", getpid());
      exit(EXIT_SUCCESS);
    }
      
  } else {
    Wait(NULL);
    ptrace(PTRACE_SYSCALL, child, 0, 0);
    monitor_clone(child);
  }
  return 0;
}

