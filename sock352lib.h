/* authors: Mark Conley && Michael Sabbagh 
 * section: 01
 */

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#define DATA_SIZE 1024

enum status {
	CONNECTED,
	UNCONNECTED,
	WAITING,
	BLOCKED
};

typedef enum status status; 

/* global structure that holds information for the connection */
struct conn_status { 
	pthread_mutex_t mutex;	
	int cli_port;
	int serv_port;
	int cid;		//connection id, incremented everytime we adda  connection
	int seq_num;
	struct sockaddr_in cliaddr;	
	struct sockaddr_in servaddr;	
	status stat;
};

typedef struct conn_status conn_status;

struct fragment {
	int size;
	sock352_pkt_hdr_t packet;
	char data[DATA_SIZE];	
};

typedef struct fragment fragment;

void destroy_conn(conn_status);
