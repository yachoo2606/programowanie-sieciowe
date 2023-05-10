/*
 *
 * Compilation:  gcc -Wall ./zadanie_04_transfer_send.c -o ./transfer_send
 * Usage:        ./transfer_send DST_IP_ADDR
 * NOTE:         This program requires root privileges.
 *
 */

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define IPPROTO 222

int main(int argc, char** argv){

    int sfd, rc, nextsocket;
    char buf[65536], saddr[16], daddr[16];
    char *data;
    socklen_t sl;
    struct sockaddr_in addr, nextaddr;
    struct iphdr *ip;

    sfd = socket(PF_INET, SOCK_RAW, IPPROTO);
    while (1){
        memset(&addr, 0, sizeof(addr));
        sl = sizeof(addr);
        rc = recvfrom(sfd, buf, sizeof(buf), 0, (struct sockaddr*)&addr, &sl);
        ip = (struct iphdr*)&buf;
        if(ip->protocol == IPPROTO){
            inet_ntop(AF_INET, &ip->saddr, (char *) &saddr, 16);
            inet_ntop(AF_INET, &ip->daddr, (char *) &daddr, 16);
            data = (char*) ip + (ip ->ihl*4);
            printf("[%dB] %s -> %s | %s\n", rc - (ip->ihl * 4), saddr, daddr, data);
            
            //SENDING TO NEXT ... SECTION

            nextsocket = socket(PF_INET, SOCK_RAW, IPPROTO);
            memset(&nextaddr, 0, sizeof(nextaddr));
            nextaddr.sin_family = AF_INET;
            nextaddr.sin_port = 0;
            nextaddr.sin_addr.s_addr = inet_addr(argv[1]);

            sendto(nextsocket, data, strlen(data) + 1, 0, (struct sockaddr*) &nextaddr, sizeof(nextaddr));
            close(nextsocket);
        }
    }
    close(sfd);
    return EXIT_SUCCESS;
}