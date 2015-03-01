#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <arpa/inet.h>

enum status {
	CONNECTED,
	UNCONNECTED,
	WAITING,
	BLOCKED
};

typedef enum status status; 

/* global structure that holds information for the connection */
typedef struct { 
	pthread_mutex_t mutex;	
	int cli_port;
	int serv_port;
	int cid;		//connection id, incremented everytime we adda  connection
	status stat;
} conn_status;

//typedef struct conn_status conn_status;

void destroy_conn(conn_status);
