#include "csapp.h"

int main(int argc, char **argv) {
  struct addrinfo *p, *listp, hints;
  char buf[MAXLINE];
  int rc, flags;

  if (argc != 3)
    app_error("usage: %s <domain name> <service name>\n", argv[0]);

  /* Get a list of addrinfo records */
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC; /* IPv4 only */
  hints.ai_socktype = 0;

  /* Connections only */
  if ((rc = getaddrinfo(argv[1], argv[2], &hints, &listp)) != 0)
    gai_error(rc, "getaddrinfo");

  /* Walk the list and display each IP address */
  flags = NI_NUMERICHOST | NI_NUMERICSERV; /* Display address string instead of domain name */
  for (p = listp; p; p = p->ai_next) {
    char serv[NI_MAXSERV];
    socklen_t servlen = NI_MAXSERV;
    Getnameinfo(p->ai_addr, p->ai_addrlen, buf, servlen, serv, servlen, flags);
    if (p->ai_family == AF_INET6)
      printf("[%s]:%s\n", buf, serv);
    else if (p->ai_family == AF_INET)
      printf("%s:%s\n", buf, serv);
    else
      assert(0);
  }


  /* Clean up */
  freeaddrinfo(listp);

  return EXIT_SUCCESS;
}
