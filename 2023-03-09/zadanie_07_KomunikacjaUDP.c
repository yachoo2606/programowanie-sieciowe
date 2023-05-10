/*
 * Compilation:  gcc -Wall ./zadanie_06_KomunikacjaTCP.c -o ./program
 * Usage:        ./program
 */

#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char** argv) {
  //reciver
  socklen_t sl; //socket_length
  int resDesc, resConDesc, on = 1,rc; // senderDescriptor, connectedDescriptor
  struct sockaddr_in ressaddr, rescaddr; //senderAddr, connectedAddr

  //sender
  int secondDesc;
  struct sockaddr_in saddr;
  struct hostent* addrent;
  
  //shared
  char buf[256];

  //Res SetUp
  memset(&ressaddr, 0, sizeof(ressaddr));
  ressaddr.sin_family = AF_INET;
  ressaddr.sin_addr.s_addr = INADDR_ANY;
  ressaddr.sin_port = htons(1234);
  resDesc = socket(PF_INET, SOCK_STREAM, 0);
  setsockopt(resDesc, SOL_SOCKET, SO_REUSEADDR, (char*) &on, sizeof(on));
  bind(resDesc, (struct sockaddr*) &ressaddr, sizeof(ressaddr));
  listen(resDesc, 5);

  //Sender Setup

  addrent = gethostbyname(argv[1]);
  secondDesc = socket(PF_INET, SOCK_DGRAM,0);
  
  while(1) {
    memset(&rescaddr, 0, sizeof(rescaddr));
    sl = sizeof(rescaddr);
    // printf("waiting for connection\n\n");
    resConDesc = accept(resDesc, (struct sockaddr*) &rescaddr, &sl);
    rc = read(resConDesc,buf,256);
    close(resConDesc);
    printf("Rec Data: %s\n",buf);

    memset(&saddr,0,sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(atoi(argv[2]));
    memcpy(&saddr.sin_addr.s_addr, addrent->h_addr, addrent->h_length);

    connect(secondDesc, (struct sockaddr*) &saddr, sizeof(saddr));
    // printf("send message\n");
    write(secondDesc, buf,rc);
    // close(secondDesc);
  }
  return EXIT_SUCCESS;
}
