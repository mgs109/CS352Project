#include "sock352.h"
#include <errno.h>

int sock352_init(int udp_port);
int sock352_socket(int domain, int type, int protocol);
int sock352_bind(int fd, sockaddr_sock352_t *addr, socklen_t len);
int sock352_connect(int fd, sockaddr_sock352_t *addr, socklen_t len);
int sock352_listen(int fd, int n);
int sock352_accept(int _fd, sockaddr_sock352_t *addr, int *len);
int sock352_close(int fd);
int sock352_read(int fd, void *buf, int count);
int sock352_write(int fd, void *buf, int count);

	

/*
 *  Takes in a single parameter, which is the UDP port that the rest of 
 *  the CS 352 RDP library will use for communication between hosts. Setting 
 *  the udp_port to zero should use default port of 27182.
 */
int sock352_init(int udp_port){
	if(udp_port == -1){
		/*Test case*/
		return 0;
	} else if(udp_port == 0){
		return SOCK352_DEFAULT_UDP_PORT;
	} else {
		if(udp_port < 0){
			return -1;
		}		
		return udp_port;
	}
}

/*
 *  Not all combinations of socket family(domain) and socket type are valid. 
 *  Figure 4.5 in the book shows valid combinations.
 *
 *  The 	third argument protocal is set to 0 in each call made from client.c and server.c
 *  so it's apparently not going to be used by us at this particular moment.
 *
 *  On success, the socket function returns a small non-negative integer value. On 
 *  failure socket functions returns -1. 
 */
int sock352_socket(int domain, int type, int protocol){
	/*if(domain != 31){
		return -1;
	}
	return 27182;
	*/	

	if(type == SOCK_STREAM){
		if(domain == AF_ROUTE || domain == AF_KEY){
			return SOCK352_FAILURE;
		}
	} else if(type == SOCK_DGRAM){
		if(domain == AF_ROUTE || domain == AF_KEY){
			return SOCK352_FAILURE;
		}
	} else if(type == SOCK_SEQPACKET){
		if(domain == AF_ROUTE || domain == AF_KEY){
			return SOCK352_FAILURE;
		}
	} else if(type == SOCK_RAW){
		if(domain == AF_LOCAL){
			return SOCK352_FAILURE;
		}
	}	

	return SOCK352_SUCCESS;
}

/*Establishes connection to TCP server. 
 * Input: fd is descriptor returned from socket.
 * 	  addr is socket address, len is its size.
 * Output: 0 on success, -1 if fails.
 */
int sock352_connect(int fd, sockaddr_sock352_t *addr, socklen_t len){
	sock352_pkt_hdr_t estConnection;
	estConnection.version = SOCK352_VER_1; 
	return ETIMEDOUT;
}

int sock352_bind(int fd, sockaddr_sock352_t *addr, socklen_t len){

	return 0;
}


int sock352_listen(int fd, int n){}
int sock352_accept(int _fd, sockaddr_sock352_t *addr, int *len){}
int sock352_close(int fd){}
int sock352_read(int fd, void *buf, int count){}
int sock352_write(int fd, void *buf, int count){}
