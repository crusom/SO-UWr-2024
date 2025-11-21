#include <unistd.h>
#include "include/csapp.h"

static int fd;
#define myexit() do { close(fd); exit(EXIT_SUCCESS); } while(0);

static int ndselect(int n) {
  /* TODO: A loop is missing here that spawns processes and waits for them! */
  // ojciec bierze kolejne dzieci 
  pid_t children[n];
  for (int i = 0; i < n; i++) {
    pid_t pid = Fork();
    children[i] = pid;
    if (pid == 0)
      return i;
  }
  // ojciec czeka na dzieci, zeby po nich posprzatac
  for (int i = 0; i < n; i++)
    waitpid(children[i], NULL, 0);

  myexit();
}

static int conflict(int x1, int y1, int x2, int y2) {
  return x1 == x2
    || y1 == y2
    || x1 + y1 == x2 + y2
    || x1 - y1 == x2 - y2;
}

static void print_line_sep(int size) {
  for (int i = 0; i < size; ++i) 
    dprintf(fd,"+---");
  dprintf(fd,"+\n");
}

static void print_board(int size, int board[size]) {
  for (int i = 0; i < size; ++i) {
    print_line_sep(size);
    for (int j = 0; j < size; ++j)
      dprintf(fd,"|%s", board[i] == j ? " Q " : "   ");
    dprintf(fd,"|\n");
  }
  print_line_sep(size);
  dprintf(fd,"\n");
}


int main(int argc, char **argv) {
  if (argc != 2)
    app_error("Usage: %s [SIZE]", argv[0]);

  int size = atoi(argv[1]);

  if (size < 4 || size > 9)
    app_error("Give board size in range from 4 to 9!");

  fd = Open("het.txt", O_RDWR | O_CREAT, 0644);

  int board[size];
  memset(board,-1,sizeof(board));
  // tworzymy wszystkie mozliwe kombinacje planszy
  for (int i = 0; i < size; i++) {
    int k = ndselect(size);
    board[i] = k;
  }

  // sprawdzamy czy działa
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      if (i == j) continue;
      if (conflict(i, board[i], j, board[j]))
        myexit();
    }
  }
  
  while(lockf(fd, F_TLOCK, 0)) {puts("locked...\n");};
  print_board(size, board);
  lockf(fd,F_ULOCK, 0);

  close(fd);
  return 0;
}
