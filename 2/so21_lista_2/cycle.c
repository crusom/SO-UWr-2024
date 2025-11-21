#include "csapp.h"

static void signal_handler(int signum, siginfo_t *info, void *data) {
  if (signum == SIGINT) {
    safe_printf("(%d) Screw you guys... I'm going home!\n", getpid());
    _exit(0);
  }
}

static void play(pid_t next, const sigset_t *set) {
  for (;;) {
    printf("(%d) Waiting for a ball!\n", getpid());
    // set is the initial mask, without blocking sigusr
    sigsuspend(set);
    // Many common uses of pause() have timing windows. The scenario involves checking a condition related to a signal and, if the signal has not occurred, calling pause().  When the  signal
    // occurs between the check and the call to pause(), the process often blocks indefinitely. The sigprocmask() and
    // sigsuspend() functions can be used to avoid this type of problem./

    /* TODO: Something is missing here! */
    usleep((300 + random() % 400) * 1000);
    Kill(next, SIGUSR1);
    printf("(%d) Passing ball to (%d)!\n", getpid(), next);
  }
}

int main(int argc, char *argv[]) {
  if (argc != 2)
    app_error("Usage: %s [CHILDREN]", argv[0]);

  int children = atoi(argv[1]);

  if (children < 4 || children > 20)
    app_error("Give number of children in range from 4 to 20!");

  struct sigaction action = {.sa_sigaction = signal_handler};
  Sigaction(SIGINT, &action, NULL);
  // register it for sigusr1 so it's not ignored
  Sigaction(SIGUSR1, &action, NULL);
  
  sigset_t mask, oldmask;
  sigemptyset(&mask);
  sigaddset(&mask, SIGUSR1);
  sigprocmask(SIG_BLOCK, &mask, &oldmask);

  pid_t last_pid;
  /* TODO: Start all processes and make them wait for the ball! */
  for (int i = 0; i < children; i++) {
    pid_t pid = Fork();
    if (pid == 0) {
      pid_t next = (i == 0) ? getppid() : last_pid;
      play(next, &oldmask);
      exit(0); //shouldn't exit i guess
    }
    last_pid = pid;
  }
  
  // pause would kinda work if we added sleep
  // we want to block sigusr1, so that it is handled when its time, and not just when parent sends kill
  // sleep(1);
  Kill(last_pid, SIGUSR1);
  //printf("passed the ball\n");
  play(last_pid, &oldmask);

  return EXIT_SUCCESS;
}
