#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if_packet.h>
#include <stdio.h>
#include <linux/in.h>
#include <linux/if_ether.h>
#include <net/if.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include <shutils.h>    // for eval()
#include <bcmnvram.h>
#include "networkmap.h"
#include "endianness.h"

unsigned char my_hwaddr[6];
unsigned char my_ipaddr[4];
unsigned char broadcast_hwaddr[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
unsigned char refresh_ip_list[255][4];
int networkmap_fullscan;
int refresh_exist_table = 0;
IP_TABLE ip_tab;
int scan_count=0;
/******** Build ARP Socket Function *********/
struct sockaddr_ll src_sockll, dst_sockll;

static int
iface_get_id(int fd, const char *device)
{
        struct ifreq    ifr;
                                                                                                                                             
                                                                                                                                             
        memset(&ifr, 0, sizeof(ifr));
        strncpy(ifr.ifr_name, device, sizeof(ifr.ifr_name));
                                                                                                                                             
                                                                                                                                             
        if (ioctl(fd, SIOCGIFINDEX, &ifr) == -1) {
                perror("iface_get_id ERR:\n");
                return -1;
        }

        return ifr.ifr_ifindex;
}
/*
 *  Bind the socket associated with FD to the given device.
 */
static int
iface_bind(int fd, int ifindex)
{
        int                     err;
        socklen_t               errlen = sizeof(err);
                                                                                                                                             
        memset(&src_sockll, 0, sizeof(src_sockll));
        src_sockll.sll_family          = AF_PACKET;
        src_sockll.sll_ifindex         = ifindex;
        src_sockll.sll_protocol        = htons(ETH_P_ARP);
                                                                                                                                             
        if (bind(fd, (struct sockaddr *) &src_sockll, sizeof(src_sockll)) == -1) {
                perror("bind device ERR:\n");
                return -1;
        }

        /* Any pending errors, e.g., network is down? */
        if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &errlen) == -1) {
                return -2;
        }
        if (err > 0) {
                return -2;
        }

        int alen = sizeof(src_sockll);
        if (getsockname(fd, (struct sockaddr*)&src_sockll, &alen) == -1) {
                perror("getsockname");
                exit(2);
        }
                                                                                                                                             
        if (src_sockll.sll_halen == 0) {
                printf("Interface is not ARPable (no ll address)\n");
                exit(2);
        }

	dst_sockll = src_sockll;

         return 0;
}

int create_socket(char *device)
{
        /* create socket */
        int sock_fd, device_id;
        sock_fd = socket(PF_PACKET, SOCK_DGRAM, 0); //2008.06.27 Yau change to UDP Socket
                                                                                                                                             
        if(sock_fd < 0)
                perror("create socket ERR:");
                                                                                                                                             
        device_id = iface_get_id(sock_fd, device);
                                                                                                                                             
        if (device_id == -1)
               printf("iface_get_id REEOR\n");
                                                                                                                                             
        if ( iface_bind(sock_fd, device_id) < 0)
                printf("iface_bind ERROR\n");
                                                                                                                                             
        return sock_fd;
}

int  sent_arppacket(int raw_sockfd, unsigned char * dst_ipaddr)
{
        ARP_HEADER * arp;

	char raw_buffer[46];

	memset(dst_sockll.sll_addr, -1, sizeof(dst_sockll.sll_addr));  // set dmac addr FF:FF:FF:FF:FF:FF                                                                                                                                              
        if (raw_buffer == NULL)
        {
                 perror("ARP: Oops, out of memory\r");
                return 1;
        }                                                                                                                          
	bzero(raw_buffer, 46);

        // Allow 14 bytes for the ethernet header
        arp = (ARP_HEADER *)(raw_buffer);// + 14);
        arp->hardware_type =htons(DIX_ETHERNET);
        arp->protocol_type = htons(IP_PACKET);
        arp->hwaddr_len = 6;
        arp->ipaddr_len = 4;
        arp->message_type = htons(ARP_REQUEST);
                                                                                                                                              
        // My hardware address and IP addresses
        memcpy(arp->source_hwaddr, my_hwaddr, 6);
        memcpy(arp->source_ipaddr, my_ipaddr, 4);
        // Destination hwaddr and dest IP addr
        memcpy(arp->dest_hwaddr, broadcast_hwaddr, 6);
        memcpy(arp->dest_ipaddr, dst_ipaddr, 4);

        if( (sendto(raw_sockfd, raw_buffer, 46, 0, (struct sockaddr *)&dst_sockll, sizeof(dst_sockll))) < 0 )
        {
                 perror("sendto");
                 return 1;
        }
	//NMP_DEBUG("Send ARP Request success to: .%d.%d\n", atoi(dst_ipaddr[2]), atoi(dst_ipaddr[3]));
	//int x = atoi(dst_ipaddr[3]);
	NMP_DEBUG_M("Send ARP Request success to: .%02X\n", dst_ipaddr[3]);
        return 0;
}
/******* End of Build ARP Socket Function ********/

/*********** Signal function **************/
static void refresh_sig(int sig)
{
        NMP_DEBUG("Refresh network map!\n");
        networkmap_fullscan = 1;
        refresh_exist_table = 0;
	scan_count = 0;	
	nvram_set("networkmap_fullscan", "1");
	nvram_commit();
#if 0
	//reset exixt ip table
        memset(&ip_tab, 0x00, sizeof(IP_TABLE));
        ip_tab.num = 0;
	//remove file;
	ret = eval("rm", "/var/networkmap.dat");
#endif
}

/******************************************/
int main()
{
	int arp_sockfd, arp_getlen, i;
	int send_count=0, exist_ip=0;
	struct sockaddr_in router_addr, device_addr;
	char router_ipaddr[17], router_mac[17], buffer[512];
	unsigned char scan_ipaddr[4]; // scan ip
	FILE *fp_ip;
	fd_set rfds;
        ARP_HEADER * arp_ptr;
        struct timeval tv1, tv2, arp_timeout;

	//Initial ip_tab
	memset(&ip_tab, 0x00, sizeof(IP_TABLE));
	ip_tab.num = 0;
	
	//Get Router's IP/Mac
	strcpy(router_ipaddr, nvram_safe_get("lan_ipaddr"));
	strcpy(router_mac, nvram_safe_get("et0macaddr"));
        inet_aton(router_ipaddr, &router_addr.sin_addr);
        memcpy(my_ipaddr,  &router_addr.sin_addr, 4);

	//Prepare scan 
        memset(scan_ipaddr, 0x00, 4);
        memcpy(scan_ipaddr, &router_addr.sin_addr, 3);
	networkmap_fullscan = 1;
	nvram_set("networkmap_fullscan", "1");
	nvram_commit();

	if (strlen(router_mac)!=0) ether_atoe(router_mac, my_hwaddr);

	signal(SIGUSR1, refresh_sig); //catch UI refresh signal

        // create UDP socket and bind to "br0" to get ARP packet//
	arp_sockfd = create_socket(INTERFACE);

        if(arp_sockfd < 0)
                perror("create socket ERR:");
	else {
	        arp_timeout.tv_sec = 0;
        	arp_timeout.tv_usec = 10000;
		setsockopt(arp_sockfd, SOL_SOCKET, SO_RCVTIMEO, &arp_timeout, sizeof(arp_timeout));//set receive timeout
		dst_sockll = src_sockll; //2008.06.27 Yau add copy sockaddr info to dst
		memset(dst_sockll.sll_addr, -1, sizeof(dst_sockll.sll_addr)); // set dmac addr FF:FF:FF:FF:FF:FF
	}

        while(1)
        {
                if(networkmap_fullscan == 1) { //Scan all IP address in the subnetwork
		    if(scan_count == 0) { // 2008/11/24, added by Chen-I to set timeout
	                arp_timeout.tv_sec = 0;
        	        arp_timeout.tv_usec = 10000;
                	setsockopt(arp_sockfd, SOL_SOCKET, SO_RCVTIMEO, &arp_timeout, sizeof(arp_timeout));//set receive timeout
			NMP_DEBUG("Starting full scan!\n");
			//2009.06.16 Yau move from signal to here
			int ret;
                        //reset exixt ip table
                        memset(&ip_tab, 0x00, sizeof(IP_TABLE));
                        ip_tab.num = 0;
                        //remove file;
                        ret = eval("rm", "/var/networkmap.dat");
		    }

		    scan_count++;
		    scan_ipaddr[3]++;
		    if( scan_count<255 && memcmp(scan_ipaddr, my_ipaddr, 4) ) {
                        sent_arppacket(arp_sockfd, scan_ipaddr);
		    }         
		    else if(scan_count>255) { //Scan completed
                	arp_timeout.tv_sec = 1;
                	arp_timeout.tv_usec = 500000; //Reset timeout at monitor state for decase cpu loading
                	setsockopt(arp_sockfd, SOL_SOCKET, SO_RCVTIMEO, &arp_timeout, sizeof(arp_timeout));//set receive timeout
			networkmap_fullscan = 0;
			//scan_count = 0;
			nvram_set("networkmap_fullscan", "0");
			nvram_commit();
			NMP_DEBUG("Finish full scan!\n");
		    }
                }
	    	/***** End of Send ARP Request******/

	    while(1) //2008/11/24, added by Chen-I, to flush recv buffer
	    {
		arp_getlen=recvfrom(arp_sockfd, buffer, 512, 0, NULL, NULL);

	   	if(arp_getlen == -1) {
                        NMP_DEBUG_M("* Recvfrom ARP Socket timeout\n");
			break;
		}
		else {
		    arp_ptr = (ARP_HEADER*)(buffer);
                    NMP_DEBUG("*Receive ARP Packet from: %02x %02x %02x %02x\n",
				arp_ptr->source_ipaddr[0],arp_ptr->source_ipaddr[1],
				arp_ptr->source_ipaddr[2],arp_ptr->source_ipaddr[3]);

		    //Check ARP packet if source ip and router ip at the same network
                    if( !memcmp(my_ipaddr, arp_ptr->source_ipaddr, 3) ) {

			//swapbytes16(arp_ptr->message_type);

			//ARP Response packet to router
			if( arp_ptr->message_type == 0x02 &&   		       	// ARP response
                       	    memcmp(arp_ptr->dest_ipaddr, my_ipaddr, 4) == 0 && 	// dest IP
                       	    memcmp(arp_ptr->dest_hwaddr, my_hwaddr, 6) == 0) 	// dest MAC
			{
			    	NMP_DEBUG("   It's ARP ResponsE Packet!\n");
			    	exist_ip = 0;
				
                        	for(i=0; i<ip_tab.num; i++) {
                              		if( !memcmp(ip_tab.ip_addr[i], arp_ptr->source_ipaddr, 4) ) {
                                       		exist_ip = 1;
                                       		break;
                                	}
                        	}

				if( !exist_ip ) {
                        		memcpy(ip_tab.ip_addr[ip_tab.num], arp_ptr->source_ipaddr, 4);
                        		memcpy(ip_tab.mac_addr[ip_tab.num], arp_ptr->source_hwaddr, 6);

					//Find all application
					FindAllApp(my_ipaddr, arp_ptr->source_ipaddr, &ip_tab);

                                	//open file and update IP/MAC info
                                	fp_ip=fopen("/var/networkmap.dat", "a");
                                	if (fp_ip==NULL) {
                                        	printf("File Open Error!\n");
                                	}
                                	else {
						memcpy(&device_addr.sin_addr, arp_ptr->source_ipaddr, 4);
                                        	fprintf(fp_ip, "%s,%02X:%02X:%02X:%02X:%02X:%02X,%s,%d,%d,%d,%d\n",
							inet_ntoa(device_addr.sin_addr),
							arp_ptr->source_hwaddr[0], arp_ptr->source_hwaddr[1], 
							arp_ptr->source_hwaddr[2], arp_ptr->source_hwaddr[3],
							arp_ptr->source_hwaddr[4], arp_ptr->source_hwaddr[5],
							ip_tab.device_name[i], ip_tab.type[i], ip_tab.http[i],
							ip_tab.printer[i], ip_tab.itune[i]);
                                	        fclose(fp_ip);
                                	}
	                        	ip_tab.num++;
				}
			}//End of ARP response to Router
			else { //Nomo ARP Packet or ARP response to other IP
	                        exist_ip = 0;
        	                //Compare IP and IP buffer if not exist
                	        if(ip_tab.num != 0) { //IP table is not empty
                        	        for(i=0; i<ip_tab.num; i++) {
                                	        if( !memcmp(ip_tab.ip_addr[i], arp_ptr->source_ipaddr, 4) ) {
                                                	NMP_DEBUG_M("Find IP in Table!\n");
        	                                        exist_ip = 1;
                	                                break;
                        	                }
                                	}
                        	}
                        	if( !exist_ip ) //Find a new IP! Send an ARP request to it
				{
					NMP_DEBUG("New IP\n");

					if(memcmp(my_ipaddr, arp_ptr->source_ipaddr, 4))
                                		sent_arppacket(arp_sockfd, arp_ptr->source_ipaddr);
					else
						NMP_DEBUG("New IP is the same as Router IP! Ignore it!\n");
				}
			}//End of Nomo ARP Packet
		    }//Source IP in the same subnetwork
		}//End of arp_getlen != -1
	    } // End of while for flush buffer
	} //End of while
	close(arp_sockfd);
	return 0;
}
