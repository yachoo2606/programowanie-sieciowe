/*
 * Copyright (C) 2023 Michal Kalewski <mkalewski at cs.put.poznan.pl>
 *
 * Compilation:  gcc -Wall ./tcp-client.c -o ./tcp-client
 * Usage:        ./tcp-client SERVER PORT
 *
 * Bug reports:  https://gitlab.cs.put.poznan.pl/mkalewski/ps-2023/issues
 *
 */

#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>

int _read(int sfd, char *buf, int bufsize) {
  int ret = bufsize;
  do {
    int i = read(sfd, buf, bufsize);
    printf("%d\n",i);
    bufsize -= i;
    buf += i;
  } while (bufsize>0);
  return ret;
}

int main(int argc, char** argv) {
  int sfd, rc;
  char buf[128];
  struct sockaddr_in saddr;
  struct hostent* addrent;

  addrent = gethostbyname(argv[1]);
  sfd = socket(PF_INET, SOCK_STREAM, 0);
  memset(&saddr, 0, sizeof(saddr));
  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(atoi(argv[2]));
  memcpy(&saddr.sin_addr.s_addr, addrent->h_addr, addrent->h_length);
  int srb = 12345;
  setsockopt(sfd, SOL_SOCKET, SO_RCVBUF, &srb, sizeof(srb));
  int ssb = 12345;
  setsockopt(sfd, SOL_SOCKET, SO_SNDBUF, &ssb, sizeof(ssb));
  connect(sfd, (struct sockaddr*) &saddr, sizeof(saddr));
  rc = _read(sfd, buf, 14);
  write(1, buf, rc);
  close(sfd);
  return EXIT_SUCCESS;
}
