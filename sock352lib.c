#include "sock352.h"
#include "sock352lib.h"

conn_status *  global_status; // = (conn_status *)malloc(sizeof(conn_status));

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
	} else if (udp_port < 0){
		return -1;		
	}

	
	global_status = (conn_status *)malloc(sizeof(conn_status));

	global_status->cli_port = 0;
	global_status->serv_port = 0;
	global_status->cid = 0;
	global_status->stat = UNCONNECTED;

	return SOCK352_SUCCESS;

}

int sock352_init2(int remote_port, int local_port){

	if(remote_port < 0 || local_port < 0){
         		return SOCK352_FAILURE;
 	}

	global_status = (conn_status *)malloc(sizeof(conn_status));

 	global_status->cli_port = 0;
	global_status->serv_port = 0;
	global_status->cid = 0;
	global_status->stat = UNCONNECTED;

 	return SOCK352_SUCCESS;
 
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
		
	return socket(AF_INET, SOCK_DGRAM, 0);
	
}

/* Establishes connection to TCP server. 
 * Input: fd is descriptor returned from socket.
 * 	  addr is socket address, len is its size.
 * Output: 0 on success, -1 if fails.
 */
int sock352_connect(int fd, sockaddr_sock352_t *addr, socklen_t len){

	struct sockaddr_in cliaddr, servaddr;	
	int n;
	//TODO: figure out where we initialize packets
	sock352_pkt_hdr_t  send;
	sock352_pkt_hdr_t  receive;
	//transfer addr data to sockaddr
	
	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(addr->sin_port);
	char* ad = "128.6.13.175"; //ip of kill.cs.rutgers.edu
	servaddr.sin_addr.s_addr = inet_addr(ad);


	global_status->serv_port = htons(addr->sin_port); //sin_port is UDP port number
	send.flags = SOCK352_SYN;


	bytes_sent = sendto(fd, &send, sizeof(send), 0, (struct sockaddr * ) &servaddr, sizeof(servaddr));

	if(bytes_sent < 0){
		printf("sendto error\n");
	} else {
		printf("%d bytes sentn", bytes_sent);
	}

	n = recvfrom(fd, &receive, sizeof(receive), 0, NULL, NULL);

	if(send.flags == (SOCK352_SYN | SOCK352_ACK)){
		printf("Success\n\n");

	}else{
		printf("just syn, need syn ack\n\n");
	}

	return connect(fd, (struct sockaddr *) &cliaddr, sizeof(cliaddr));
}

/* Assigns protocol address to socket.
 * Input: fd is descriptor returned from socket.
 * 	  addr is pointed to a protocol specific address.
 * 	  len is length of addr.
 * Output: 0 if OK, -1 if error.
 */
int sock352_bind(int fd, sockaddr_sock352_t *addr, socklen_t len){
	//Transfer sock352 addr data to a sockaddr
	struct sockaddr_in myaddr;
	myaddr.sin_family = AF_INET;
	myaddr.sin_port = htons(addr->sin_port);
	myaddr.sin_addr.s_addr = htonl(addr->sin_addr.s_addr);
	socklen_t mylen = sizeof(myaddr);
	return 0;
	return  bind(fd,  (struct sockaddr *) &myaddr, mylen);

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
	//listen can return 0 for part 1
	return 0;
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
	//transfer addr data

	struct sockaddr_in cliaddr, servaddr;	
	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(addr->sin_port);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	socklen_t mylen = sizeof(cliaddr);
	
	sock352_pkt_hdr_t mybuff[sizeof(sock352_pkt_hdr_t)];
	int mybufflen = sizeof(mybuff);
	
	int n;

	n = recvfrom(_fd, mybuff, mybufflen, 0, (struct sockaddr *) &cliaddr,  &mylen);

	//printf("FD: %d, addr port: %d, addrl: %d", _fd, servaddr.sin_port, servaddr.sin_addr.s_addr);	
	//receive packet. stored in mybuff.
	//set up ack and seq numbers
	if(mybuff->flags == SOCK352_SYN){
		mybuff->flags = (SOCK352_SYN | SOCK352_ACK);

		sendto(_fd, mybuff, n, 0, (struct sockaddr * ) &cliaddr, sizeof(cliaddr));
	}
	else{
		return 1;

	}
	//create empty list of fragments
		
	return accept(_fd, (struct sockaddr * ) &servaddr, (int * ) &mylen);
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
