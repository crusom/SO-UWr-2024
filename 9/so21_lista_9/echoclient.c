#include "csapp.h"
#include "rio.h"

int main(int argc, char **argv) {
  if (argc != 3)
    app_error("usage: %s <host> <port>\n", argv[0]);

  char *host = argv[1];
  char *port = argv[2];

  char buf[MAXLINE];
  rio_t rio;
  int clientfd;

  clientfd = Open_clientfd(host, port);
  setsockopt(clientfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
  rio_readinitb(&rio, clientfd);

  while (Fgets(buf, MAXLINE, stdin) != NULL) {
    Rio_writen(clientfd, buf, strlen(buf));
    /* XXX: Uncomment line below and watch server behaviour. */
    //exit(0);
    Rio_readlineb(&rio, buf, MAXLINE);
    Fputs(buf, stdout);
  }
  Close(clientfd);

  return EXIT_SUCCESS;
}
