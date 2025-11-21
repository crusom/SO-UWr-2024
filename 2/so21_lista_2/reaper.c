#include "csapp.h"
extern char **environ;  // Declare access to the current environment.

static pid_t spawn(void (*fn)(void)) {
  pid_t pid = Fork();
  if (pid == 0) {
    fn();
    printf("(%d) I'm done!\n", getpid());
    exit(EXIT_SUCCESS);
  }
  return pid;
}


static void catch_function(int signo) {
  printf("(%d) Got the signal!\n", getpid());
  exit(signo);
}

static void grandchild(void) {
  printf("(%d) Waiting for signal!\n", getpid());
  /* TODO: Something is missing here! */
//  if (signal(SIGINT, catch_function) == SIG_ERR) {
//    fputs("An error occurred while setting a signal handler.\n", stderr);
//    exit(1);
//  }
  pause();
}

static void child(void) {
  setpgid(0,0);
  pid_t pid = spawn(grandchild);
  /* TODO: Spawn a child! */
  printf("(%d) Grandchild (%d) spawned!\n", getpid(), pid);
}

/* Runs command "ps -o pid,ppid,pgrp,stat,cmd" using execve(2). */
static void ps(void) {
  /* TODO: Something is missing here! */
  pid_t pid;
  if((pid = fork()) == 0) {
    char *argv[] = {"ps", "-o", "pid,ppid,pgrp,stat,cmd", NULL};
    if(execve("/bin/ps", argv, environ)) fprintf(stderr, "Execve returned!!!!!!?/1/1!/\n");
  }
  waitpid(pid, NULL, 0);
}
int main(void) {
  /* TODO: Make yourself a reaper. */
  Prctl(PR_SET_CHILD_SUBREAPER, 1);
  printf("(%d) I'm a reaper now!\n", getpid());

  pid_t pid, pgrp;
  int status;

  /* TODO: Start child and grandchild, then kill child!
   * Remember that you need to kill all subprocesses before quit. */
  pid_t child_pid = spawn(child);
  waitpid(child_pid, &status, 0);
  ps();
  // negative indicates group!
  kill(-child_pid, SIGINT);

  pid = wait(&status);
  printf("(%d) Exited with status %d\n", pid, WEXITSTATUS(status));

  return EXIT_SUCCESS;
}
