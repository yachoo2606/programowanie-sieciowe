#include <arpa/inet.h>
#include <linux/if.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char **argv){
    int sockDescr;
    struct ifreq ifr;

    sockDescr = socket(PF_INET, SOCK_DGRAM,0);
    strncpy(ifr.ifr_name, argv[1], IFNAMSIZ);

    ioctl(sockDescr, SIOCGIFFLAGS, &ifr);
    
    if(atoi(argv[2])==1){
        ifr.ifr_flags |=IFF_PROMISC;
        ioctl(sockDescr,SIOCSIFFLAGS, &ifr);
        printf("%s turn ON promiscuous.\n",argv[1]);
    }else if(atoi(argv[2])==0){
        ifr.ifr_flags &= ~IFF_PROMISC;
        ioctl(sockDescr, SIOCSIFFLAGS, &ifr);
        printf("%s turn OFF promiscuous.\n", argv[1]);
    }else{
        printf("Invalid arguments. Use:\n%s <interface> 1 to turn on the interface.\n%s <interface> 0 to turn down the interface.\n", argv[0], argv[0]);
        exit(1);
    }
    exit(0);
}