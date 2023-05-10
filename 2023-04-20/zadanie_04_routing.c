/*
 *
 *
 * Compilation:  gcc -Wall ./ethrecv.c -o ./ethrecv
 * Usage:        ./ethrecv INTERFACE
 * NOTE:         This program requires root privileges.
 *
 *
 */

#include <arpa/inet.h>
#include <linux/if_arp.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <linux/route.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define ETH_P_CUSTOM 0x8888
#define IRI_T_ADDRESS 0
#define IRI_T_ROUTE 1

struct ifrtinfo {
  int iri_type;                 /* msg type  */
  char iri_iname[16];           /* ifname  */
  struct sockaddr_in iri_iaddr; /* IP address  */
  struct sockaddr_in iri_rtdst; /* dst. IP address */
  struct sockaddr_in iri_rtmsk; /* dst. netmask  */
  struct sockaddr_in iri_rtgip; /* gateway IP */
};

int main(int argc, char** argv) {
  int sfd, i;
  ssize_t len;
  char* frame;
  char* fdata;
  struct ethhdr* fhead;
  struct ifreq ifr;
  struct sockaddr_ll sall;

  struct ifrtinfo* r;

  sfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_CUSTOM));
  strncpy(ifr.ifr_name, argv[1], IFNAMSIZ);
  ioctl(sfd, SIOCGIFINDEX, &ifr);
  memset(&sall, 0, sizeof(struct sockaddr_ll));
  sall.sll_family = AF_PACKET;
  sall.sll_protocol = htons(ETH_P_CUSTOM);
  sall.sll_ifindex = ifr.ifr_ifindex;
  sall.sll_hatype = ARPHRD_ETHER;
  sall.sll_pkttype = PACKET_HOST;
  sall.sll_halen = ETH_ALEN;
  bind(sfd, (struct sockaddr*)&sall, sizeof(struct sockaddr_ll));
  while (1) {
    frame = malloc(ETH_FRAME_LEN);
    memset(frame, 0, ETH_FRAME_LEN);
    fhead = (struct ethhdr*)frame;
    // fdata = frame + ETH_HLEN;
    len = recvfrom(sfd, frame, ETH_FRAME_LEN, 0, NULL, NULL);
    r = (struct ifrtinfo*)(frame + ETH_HLEN);

    printf("Type: %d\n", r->iri_type);
    if (r->iri_type == IRI_T_ADDRESS) {
      printf("[IRI_T_ADDRESS]\n");

      int sfd;
      struct ifreq ifr;
      struct sockaddr_in* sin;

      sfd = socket(PF_INET, SOCK_DGRAM, 0);
      strncpy(ifr.ifr_name, r->iri_iname, strlen(r->iri_iname) + 1);
      sin = (struct sockaddr_in*)&ifr.ifr_addr;
      memset(sin, 0, sizeof(struct sockaddr_in));
      sin->sin_family = AF_INET;
      sin->sin_port = 0;
      sin->sin_addr.s_addr = r->iri_iaddr.sin_addr.s_addr;
      ioctl(sfd, SIOCSIFADDR, &ifr);
      ioctl(sfd, SIOCGIFFLAGS, &ifr);
      ifr.ifr_flags |= IFF_UP | IFF_RUNNING;
      ioctl(sfd, SIOCSIFFLAGS, &ifr);
      close(sfd);

    } else if (r->iri_type == IRI_T_ROUTE) {
      printf("[IRI_T_ROUTE]\n");

      int sfd;
      struct rtentry route;
      struct sockaddr_in* addr;

      sfd = socket(PF_INET, SOCK_DGRAM, 0);
      memset(&route, 0, sizeof(route));
      addr = (struct sockaddr_in*)&route.rt_gateway;
      addr->sin_family = AF_INET;
      addr->sin_addr.s_addr = r->iri_rtgip.sin_addr.s_addr;
      addr = (struct sockaddr_in*)&route.rt_dst;
      addr->sin_family = AF_INET;
      addr->sin_addr.s_addr = r->iri_rtdst.sin_addr.s_addr;
      addr = (struct sockaddr_in*)&route.rt_genmask;
      addr->sin_family = AF_INET;
      addr->sin_addr.s_addr = r->iri_rtmsk.sin_addr.s_addr;
      route.rt_flags = RTF_UP | RTF_GATEWAY;
      route.rt_metric = 0;
      ioctl(sfd, SIOCADDRT, &route);
      close(sfd);
    }
    free(frame);
  }
  close(sfd);
  return EXIT_SUCCESS;
}