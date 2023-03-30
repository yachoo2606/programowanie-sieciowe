#include <arpa/inet.h>
#include <linux/if_arp.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define ETH_P_CUSTOM 0x8888


int main(int argc, char** argv) {
  int sfd, sfd2, i, ifindex;
  ssize_t len;
  char* frame;
  char* frame2;
  char* fdata;
  char* fdata2;
  struct ethhdr* fhead;
  struct ethhdr* fhead2;
  struct ifreq ifr;
  struct ifreq ifr2;
  struct sockaddr_ll sall;
  struct sockaddr_ll sall2;

  sfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_CUSTOM));
  strncpy(ifr.ifr_name, argv[1], IFNAMSIZ);
  ioctl(sfd, SIOCGIFINDEX, &ifr);
  memset(&sall, 0, sizeof(struct sockaddr_ll));
  sall.sll_family = AF_PACKET;
  sall.sll_protocol = htons(ETH_P_CUSTOM);
  sall.sll_ifindex = ifr.ifr_ifindex;
  sall.sll_hatype = ARPHRD_ETHER;
  sall.sll_halen = ETH_ALEN;
  bind(sfd, (struct sockaddr*) &sall, sizeof(struct sockaddr_ll));
  while(1) {
    frame = malloc(ETH_FRAME_LEN);
    memset(frame, 0, ETH_FRAME_LEN);
    fhead = (struct ethhdr*) frame;
    fdata = frame + ETH_HLEN;
    len = recvfrom(sfd, frame, ETH_FRAME_LEN, 0, NULL, NULL);
    printf("[%dB]  packet_type: %u  EtherType: %#06x\n", (int)len, sall.sll_pkttype, ntohs(fhead->h_proto));
    printf("%02x:%02x:%02x:%02x:%02x:%02x -> ",
           fhead->h_source[0], fhead->h_source[1], fhead->h_source[2],
           fhead->h_source[3], fhead->h_source[4], fhead->h_source[5]);
    printf("%02x:%02x:%02x:%02x:%02x:%02x | ",
           fhead->h_dest[0], fhead->h_dest[1], fhead->h_dest[2],
           fhead->h_dest[3], fhead->h_dest[4], fhead->h_dest[5]);
    printf("%s\n", fdata);
    for (i = 0; i < len ; i++) {
      printf("%02x ", (unsigned char) frame[i]);
      if ((i + 1) % 16 == 0)
        printf("\n");
    }
    printf("\n\n");

    sfd2 = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_CUSTOM));
    frame2 = malloc(ETH_FRAME_LEN);
    memset(frame2, 0, ETH_FRAME_LEN);
    fhead2 = (struct ethhdr*) frame2;
    fdata2 = frame2 + ETH_HLEN;
    strncpy(ifr2.ifr_name, argv[1], IFNAMSIZ);
    ioctl(sfd2, SIOCGIFINDEX, &ifr2);
    ifindex = ifr2.ifr_ifindex;
    ioctl(sfd2, SIOCGIFHWADDR, &ifr2);
    memset(&sall2, 0, sizeof(struct sockaddr_ll));
    sall2.sll_family = AF_PACKET;
    sall2.sll_protocol = htons(ETH_P_CUSTOM);
    sall2.sll_ifindex = ifindex;
    sall2.sll_hatype = ARPHRD_ETHER;
    sall2.sll_pkttype = PACKET_OUTGOING;
    sall2.sll_halen = ETH_ALEN;
    sscanf(argv[2], "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
         &sall2.sll_addr[0], &sall2.sll_addr[1], &sall2.sll_addr[2],
         &sall2.sll_addr[3], &sall2.sll_addr[4], &sall2.sll_addr[5]);
    memcpy(fhead2->h_dest, &sall2.sll_addr, ETH_ALEN);
    memcpy(fhead2->h_source, &ifr2.ifr_hwaddr.sa_data, ETH_ALEN);
    fhead2->h_proto = htons(0x8888);
    memcpy(fdata2, fdata, strlen(fdata) + 1);
    sendto(sfd2, frame2, ETH_HLEN + strlen(fdata2) + 1, 0,
         (struct sockaddr*) &sall2, sizeof(struct sockaddr_ll));
    free(frame);
    free(frame2);
    close(sfd2);
  
  }
  close(sfd);
  return EXIT_SUCCESS;
}