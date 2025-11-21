 #include <signal.h>
 #include <string.h>
 #include <fcntl.h>
 #include <stdint.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <sys/types.h>
 #include <unistd.h>
 #include <errno.h>

 int
 main(void) {

  pid_t pid;

  if (signal(SIGCHLD, SIG_IGN) == SIG_ERR) {
      perror("signal");
      exit(EXIT_FAILURE);
  }
  pid = fork();
  switch (pid) {
  case -1:
      perror("fork");
      exit(EXIT_FAILURE);
  case 0:
      puts("Child: living my best life :)");
      printf("Child: my parent is %u\n", getppid());
      sleep(2);
      printf("Child: my parent died, now my ppid is %u\n", getppid());
      puts("Child exiting.");
      exit(EXIT_SUCCESS);
  default:
      printf("Child is PID %jd\n", (intmax_t) pid);
      puts("Parent: exiting.");
      exit(EXIT_SUCCESS);
  }
 }

