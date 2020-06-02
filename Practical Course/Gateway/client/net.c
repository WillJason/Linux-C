#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>

//网络初始化
int net_init(char *server_ip,unsigned short portvalue){
	int Net_fd;
	struct sockaddr_in s_add, c_add;
	unsigned short portnum = portvalue;
	
	Net_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == Net_fd)
	{
		printf("socket fail ! \r\n");
		return -1;
	}
	//fcntl(Net_fd,F_SETFL,O_NONBLOCK);
	
	printf("socket ok !\r\n");

	bzero(&s_add,sizeof(struct sockaddr_in));
	s_add.sin_family=AF_INET;
	s_add.sin_addr.s_addr= inet_addr(server_ip);
	s_add.sin_port=htons(portnum);
	printf("s_addr = %#x ,port : %#x\r\n",s_add.sin_addr.s_addr,s_add.sin_port);

	if(-1 == connect(Net_fd,(struct sockaddr *)(&s_add), sizeof(struct sockaddr)))
	{
		printf("connect fail !\r\n");
		return -1;
	}

	printf("connect ok !\r\n");
	return Net_fd;
}