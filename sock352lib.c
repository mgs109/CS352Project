#include "sock352.h"
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>

int sock352_init(int udp_port);
int sock352_socket(int domain, int type, int protocol);
int sock352_bind(int fd, sockaddr_sock352_t *addr, socklen_t len);
int sock352_connect(int fd, sockaddr_sock352_t *addr, socklen_t len);
int sock352_listen(int fd, int n);
int sock352_accept(int _fd, sockaddr_sock352_t *addr, int *len);
int sock352_close(int fd);
int sock352_read(int fd, void *buf, int count);
int sock352_write(int fd, void *buf, int count);

sock352_pkt_hdr_t * init_packet_hdr(uint32_t clientPort, in_port_t destinationPort){
	sock352_pkt_hdr_t  * estConnection;
	estConnection = (sock352_pkt_hdr_t * ) malloc(sizeof(sock352_pkt_hdr_t));
	estConnection->version = SOCK352_VER_1; 
	estConnection->flags = 0;
	estConnection->opt_ptr = 0;
	estConnection->protocol = 0;
	estConnection->header_len = sizeof(sock352_pkt_hdr_t); 
	estConnection->checksum = 0;
	estConnection->source_port = clientPort;
	estConnection->dest_port = destinationPort;
	estConnection->sequence_no = 0;
	estConnection->ack_no = 0;
	estConnection->window = 0;
	estConnection->payload_len = 0;

	return estConnection;
}

int attempt_syn(sock352_pkt_hdr_t * packet){
	if(packet->ack_no == 0){
		packet->ack_no = SOCK352_SYN;
		return 0;
	} else {
		return -1;
	}
}

int attempt_acksyn(sock352_pkt_hdr_t * packet){
	if(packet->ack_no == SOCK352_SYN){
		packet->ack_no = SOCK352_SYN + SOCK352_ACK;
		return 0;
	} else {
		return -1;
	}
}

int attempt_ack(sock352_pkt_hdr_t * packet){
	if(packet->ack_no == (SOCK352_SYN + SOCK352_ACK)){
		packet->ack_no = SOCK352_ACK;
		return 0;
	} else {
		return -1;
	}
}

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
 *  The	third argument protocal is set to 0 in each call made from client.c and server.c
 *  so it's apparently not going to be used by us at this particular moment.
 *
 *  On success, the socket function returns a small non-negative integer value. On 
 *  failure socket functions returns -1. 
 */
int sock352_socket(int domain, int type, int protocol){

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

	int a = socket(domain, SOCK_DGRAM, protocol);
	perror("Error:" );
	return a; 
}

/* Establishes connection to TCP server. 
 * Input: fd is descriptor returned from socket.
 * 	  addr is socket address, len is its size.
 * Output: 0 on success, -1 if fails.
 */
int sock352_connect(int fd, sockaddr_sock352_t *addr, socklen_t len){
	
	//TODO: figure out where we initialize packets
/*	sock352_pkt_hdr_t * connection = init_packet_hdr(addr->cs352_port, addr->sin_port);
	if(attempt_syn(connection) != 0 ){
		return ETIMEDOUT;
	}
*/
	return connect(fd, (void *)&addr, len);
}

/* Assigns protocol address to socket.
 * Input: fd is descriptor returned from socket.
 * 	  addr is pointed to a protocol specific address.
 * 	  len is length of addr.
 * Output: 0 if OK, -1 if error.
 */
int sock352_bind(int fd, sockaddr_sock352_t *addr, socklen_t len){
//	addr->cs352_port = htonl(INADDR_ANY);
	int a =  bind(fd, (void *) &addr, len);
	perror("Error:");
	return a;
}

/* Called by server, performs 2 actions.
 * 1. Converts an unconnected socket into a passive socket. 
 * 	Indicates that kernel should accept incoming connection
 * 	requests directed to this socket. Moves socket from CLOSED
 * 	state to LISTEN state.
 * 2. Specifies maximum number of connections kernel should queue for
 * 	the socket (second argument, n).
 *
 * Output: 0 if OK, -1 if error.
 *
 */
int sock352_listen(int fd, int n){

	char* ptr;
	if((ptr = getenv("LISTENQ")) != NULL){
		n = atoi(ptr);
	}

	return listen(fd, n);
}

/* Called by server. Returns next completed connection from the front of 
 * connection queue. If Q is empty, process put to sleep/
 * Inputs: addr and len are used to return the protocol address of the client.
 * 	   len should be set to size of the socket address structure addr points
 * 	   to. 
 *
 *
 * Output: non-neg descriptor if OK, -1 if error.
 */
int sock352_accept(int _fd, sockaddr_sock352_t *addr, int *len){
	return accept(_fd, (void *) &addr, (int * ) &len);
}
int sock352_close(int fd){
	return close(fd);
}
int sock352_read(int fd, void *buf, int count){
	return read(fd, (void * ) &buf, count);
}
int sock352_write(int fd, void *buf, int count){
	return write(fd, (void *) &buf, count);
}
