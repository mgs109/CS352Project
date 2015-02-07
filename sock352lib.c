#include "sock352.h"

int sock352_init(int udp_port);
int sock352_socket(int domain, int type, int protocol);
int sock352_bind(int fd, sockaddr_sock352_t *addr, socklen_t len);
int sock352_connect(int fd, sockaddr_sock352_t *addr, socklen_t len);
int sock352_listen(int fd, int n);
int sock352_accept(int _fd, sockaddr_sock352_t *addr, int *len);
int sock352_close(int fd);
int sock352_read(int fd, void *buf, int count);
int sock352_write(int fd, void *buf, int count);


int main(int argc, char** argv){
	return 0;
}


int sock352_init(int udp_port){
	if(udp_port == -1){
		/*Test case*/
		return 0;
	}else{
		if(udp_port < 0){
			return -1;
		}		
		return udp_port;
	}
}


int sock352_socket(int domain, int type, int protocol){
	if(domain != 31){
		return -1;
	}
	return 27182;
}


int sock352_connect(int fd, sockaddr_sock352_t *addr, socklen_t len){

	return 0;
}

int sock352_bind(int fd, sockaddr_sock352_t *addr, socklen_t len){

	return 0;
}
