#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
 
#include <stdio.h>
 
int _write(int sfd, char *buf, int len) {
  while (len > 0) {
    int i = write(sfd, buf, len);
    printf("%d\n",i);
    len -= i;
    buf += i;
  }
  return len;
}
 
int main(int argc, char** argv) {
  socklen_t sl;
  int sfd, cfd, on = 1;
  struct sockaddr_in saddr, caddr;
  memset(&saddr, 0, sizeof(saddr));
  saddr.sin_family = AF_INET;
  saddr.sin_addr.s_addr = INADDR_ANY;
  saddr.sin_port = htons(1234);
  sfd = socket(PF_INET, SOCK_STREAM, 0);
  setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (char*) &on, sizeof(on));
  bind(sfd, (struct sockaddr*) &saddr, sizeof(saddr));
  listen(sfd, 5);
  while(1) {
    memset(&caddr, 0, sizeof(caddr));
    sl = sizeof(caddr);
    cfd = accept(sfd, (struct sockaddr*) &caddr, &sl);
    _write(cfd, "Hello World!\n", 14);
    close(cfd);
  }
  close(sfd);
  return EXIT_SUCCESS;
}