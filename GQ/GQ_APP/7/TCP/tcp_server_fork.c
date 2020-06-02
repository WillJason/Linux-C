#include <stdlib.h> 
#include <stdio.h> 
#include <errno.h> 
#include <string.h> 
#include <netdb.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 

#define MY_PORT 3333

int main(int argc ,char **argv)
{
	int listen_fd,accept_fd;
	struct sockaddr_in     client_addr;
	int n;
	int nbytes;
 
 	if((listen_fd=socket(AF_INET,SOCK_STREAM,0))<0)
  	{
        	printf("Socket Error:%s\n\a",strerror(errno));
        	exit(1);
  	}
 
 	bzero(&client_addr,sizeof(struct sockaddr_in));
 	client_addr.sin_family=AF_INET;
 	client_addr.sin_port=htons(MY_PORT);
 	client_addr.sin_addr.s_addr=htonl(INADDR_ANY);
 	n=1;
 
 	/* 如果服务器终止后,服务器可以第二次快速启动而不用等待一段时间  */
 	setsockopt(listen_fd,SOL_SOCKET,SO_REUSEADDR,&n,sizeof(int));
 	if(bind(listen_fd,(struct sockaddr *)&client_addr,sizeof(client_addr))<0)
  	{
        	printf("Bind Error:%s\n\a",strerror(errno));
        	exit(1);
  	}
  
  	listen(listen_fd,5);
  	
  	while(1)
  	{
   		accept_fd=accept(listen_fd,NULL,NULL);
   		if((accept_fd<0)&&(errno==EINTR))
          		continue;
   		else if(accept_fd<0)
    		{
        		printf("Accept Error:%s\n\a",strerror(errno));
        		continue;
    		}
  		if((n=fork())==0)
   		{
        		/* 子进程处理客户端的连接 */
        		char buffer[1024];

        		if((nbytes=read(accept_fd,buffer,1024))==-1) 
			{ 
				fprintf(stderr,"Read Error:%s\n",strerror(errno)); 
				exit(1); 
			} 		
			buffer[nbytes]='\0';
			printf("Server received %s\n",buffer);
        		
        		close(listen_fd);
        		close(accept_fd);
        		exit(0);
   		}
   		else 
   			close(accept_fd);
	}
} 