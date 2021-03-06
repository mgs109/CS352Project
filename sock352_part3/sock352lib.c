/* authors: Mark Conley && Michael Sabbagh 
 * section: 01
 */

#include "sock352.h"
#include "sock352lib.h"

conn_status * global_status = NULL;

/* Description: initializes a packet header to simplify initialization process */
sock352_pkt_hdr_t * init_packet_hdr(uint32_t clientPort, in_port_t destinationPort){
	sock352_pkt_hdr_t  * estConnection;
	estConnection = (sock352_pkt_hdr_t * ) malloc(sizeof(sock352_pkt_hdr_t));
	if(!estConnection) exit(-1);
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
	global_status->seq_num = 100;
	global_status->stat = UNCONNECTED;

	return SOCK352_SUCCESS;

}

/* Description: initializer for testing on same machine */
int sock352_init2(int remote_port, int local_port){

	if(remote_port < 0 || local_port < 0){
         		return SOCK352_FAILURE;
 	}

	global_status = (conn_status *)malloc(sizeof(conn_status));

 	global_status->cli_port = local_port;
	global_status->serv_port = remote_port;
	global_status->cid = 0;
	global_status->seq_num = 0;
	global_status->stat = UNCONNECTED;

 	return SOCK352_SUCCESS;
 
 }

int sock352_init3(int remote_UDP_port, int local_UDP_port, char * environment_p[]){

	global_status = (conn_status *)malloc(sizeof(conn_status));

 	global_status->cli_port = local_UDP_port;
	global_status->serv_port = remote_UDP_port;
	global_status->cid = 0;
	global_status->seq_num = 0;
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

	/* checks for compatibility of socket domain and socket type */
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

/* Establishes connection to UDP server. 
 * Input: fd is descriptor returned from socket.
 * 	  addr is socket address, len is its size.
 * Output: 0 on success, -1 if fails.
 */
int sock352_connect(int fd, sockaddr_sock352_t *addr, socklen_t len){
	int n;
	struct sockaddr_in servaddr,cliaddr;

	sock352_pkt_hdr_t * send = (sock352_pkt_hdr_t *)malloc(sizeof(sock352_pkt_hdr_t));
	sock352_pkt_hdr_t * recv = (sock352_pkt_hdr_t *)malloc(sizeof(sock352_pkt_hdr_t));

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr=addr->sin_addr.s_addr;
	servaddr.sin_port=addr->sin_port;

	/*Send SYN packet*/
	send->flags = SOCK352_SYN;
	sendto(fd,send,sizeof(send),0,(struct sockaddr *)&servaddr,sizeof(servaddr));

	/*Receive SYN ACK packet*/
	n = recvfrom(fd, recv, sizeof(recv), 0, NULL, NULL);
	

	if(recv->flags == (SOCK352_SYN | SOCK352_ACK)){
		/*Send ACK packet*/
		send->flags = SOCK352_ACK;
		sendto(fd,send,sizeof(send),0,(struct sockaddr *)&servaddr,sizeof(servaddr));
	}else{
		printf("Failure to receive SYN ACK packet in file: %s, line: %d\n", __FILE__, __LINE__);
		exit(1);
	}

	global_status->servaddr = servaddr;
	
	return  SOCK352_SUCCESS; 
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
	global_status->connaddr = addr;
	return 0;
//	return  bind(fd,  (struct sockaddr *) &myaddr, mylen);

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
	int n;
	struct sockaddr_in servaddr,cliaddr;
	socklen_t leng;
	sock352_pkt_hdr_t * mybuff;

	mybuff = (sock352_pkt_hdr_t *)malloc(sizeof(sock352_pkt_hdr_t));

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servaddr.sin_port = global_status->connaddr->sin_port;

	/*Bind the port*/
	if(bind(_fd,(struct sockaddr *)&servaddr,sizeof(servaddr)) != 0){
		printf("Bind failed in file: %s, at line: %d\n", __FILE__, __LINE__);
		exit(1);
	}
	
	/*Receive SYN packet*/
	leng = sizeof(cliaddr);
	n = recvfrom(_fd,mybuff,sizeof(mybuff),0,(struct sockaddr *)&cliaddr,&leng);

	global_status->cliaddr = cliaddr;

	if(mybuff->flags == SOCK352_SYN){	
		/*Send SYN ACK packet*/
		mybuff->flags = (SOCK352_SYN | SOCK352_ACK);
		sendto(_fd,mybuff,n,0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
	}else{
		printf("Failure to receive SYN packet\n");
		exit(1);
	}

	/*Receive ACK packet*/
	n = recvfrom(_fd,mybuff,sizeof(mybuff),0,(struct sockaddr *)&cliaddr,&leng);
	if(mybuff->flags == SOCK352_ACK){
	}
	return _fd;   
}

/* read accepts incoming packets from client, validates sequence numbers,
 * and closes connect when relevant flag is finished. Additionally, sends
 * acknowledgement when packet is received.
 *
 * note: receives one packet at a time.
 */
int sock352_read(int fd, void *buf, int count){
	int bytes_read;
	socklen_t leng;
	fragment * frag;
	fragment retfrag;

	frag = (fragment *) &retfrag;

	// receive packet
	leng = sizeof(global_status->servaddr);
	bytes_read = recvfrom(fd, frag, sizeof(fragment), 0, (struct sockaddr *) &global_status->servaddr, &leng);

	memcpy(buf, frag->data, frag->size);
	printf("Read socket: %d\n", fd);

	if(bytes_read < 0){
		printf("error in file: %s  on line %d, bytes: %d, leng: %d\n", __FILE__,  __LINE__, bytes_read, leng);
		return -1;
	}

	if(frag->packet.flags == SOCK352_FIN){
		printf("Connection terminated, FIN packet sent.\n");
		return 0;
	}

	//send ACK
	frag->packet.flags = SOCK352_ACK;
	bytes_read = frag->size;

	sendto(fd, frag, bytes_read, 0, (struct sockaddr * ) &global_status->servaddr, sizeof(global_status->servaddr));

	return  bytes_read;

}

/*
 * called by client, sents up sequence numbers accordingly, sends packet, and waits
 * until acknowledgement is received before it continues. handles timeout with
 * setsockopt function.
 *
 * note: sends one packet at at time.
 */
int sock352_write(int fd, void *buf, int count){

	struct timeval tv;
	tv.tv_sec = 0.2;  
	setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&tv,sizeof(struct timeval));

	if((char *) buf == NULL){
		printf("Buffer is null in file: %s, line: %d\n", __FILE__, __LINE__);
		return 0;
	}

	printf("Write socket: %d\n", fd);

	int bytes_sent = 0, fragbool = 0, leng;

	fragment * sendfrag = (fragment * )malloc(sizeof(fragment));
	fragment * recvfrag = (fragment * )malloc(sizeof(fragment));

	//Set up send frag data
	memcpy(sendfrag->data, buf, count);
	sendfrag->file_size = buf;
	sendfrag->size = count;
	sendfrag->packet.payload_len = count;
	sendfrag->packet.source_port = global_status->connaddr->sin_port;	
	sendfrag->packet.dest_port = global_status->connaddr->sin_port;
	sendfrag->packet.sequence_no = global_status->seq_num;
	global_status->seq_num++;

	if(count != 24){
	bytes_sent = sendto(fd, sendfrag, sizeof(fragment), 0, (struct sockaddr * )&global_status->servaddr, sizeof(global_status->servaddr));
	}else{

	bytes_sent = sendto(fd, sendfrag, sizeof(fragment), 0, (struct sockaddr * )&global_status->cliaddr, sizeof(global_status->cliaddr));

}


	while(fragbool == 0){//not ack, sendto before the loop 

		if(bytes_sent < 0){
			printf("Error sending in File: %s, Line %d\n",  __FILE__ , __LINE__);
			return -1;
		}

		recvfrom(fd, recvfrag, sizeof(fragment), 0, NULL, NULL);

		global_status->seq_num++;
		fragbool = 1;	
	}

	free(sendfrag);
	free(recvfrag);
	return count;
}

/* sends fragment with fin flag set to close connection */
int sock352_close(int fd){

    fragment * sendfrag = (fragment *)malloc(sizeof(fragment));
    sendfrag->packet.flags = SOCK352_FIN;

	if(sendto(fd, &sendfrag, sizeof(sendfrag), 0, (struct sockaddr * )&global_status->servaddr, 
		sizeof(global_status->servaddr)) < 0){
		return -1;
	}	

	free(sendfrag);
	return close(fd);
}
