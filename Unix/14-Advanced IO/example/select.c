//下面是示例代码：
//代码是服务器TCP模型，采用多路复用的select函数实现了循环的监听并接受客户端的功能，其中也包含了上传下载的功能*/

//--/server.c
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<string.h>
#include<netinet/in.h>
#include<sys/ioctl.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/types.h>
#include<dirent.h>
 

int main()
{
    struct sockaddr_in seraddr,cliaddr;
    int listenfd,connfd,fd1,fd2,n,m,l,port;
    char user[20],buf[4096];
    char *dir[100]={};

    fd_set  readfds,tmpfds;//岗哨监控集合
    socklen_t addrlen;
    DIR *dr;
    struct dirent *file;
 
    printf("请输入需要设定的服务器名称:");
    scanf("%s",user);
     
    printf("请输入需要设定的服务器端口:");
    scanf("%d",&port);
    getchar();
 
    if((listenfd=socket(AF_INET,SOCK_STREAM,0))<0)
    {
        perror("创建失败");
       exit(-1);
    }
     
     
 
    /*开始设定服务器的参数地址类型,IP,PORT*/
    memset(&seraddr,0,sizeof(seraddr));//将服务器的初值空间清空，防止转化过程有影响
    seraddr.sin_family=AF_INET;
    seraddr.sin_port=htons(port);//将得到的本地端口转换为网络字节序
    seraddr.sin_addr.s_addr=htonl(INADDR_ANY);//将得到的ip地址字符串转换为网络字节序的ip地址数值
 
    if((bind(listenfd,(struct sockaddr*)&seraddr,sizeof(seraddr))<0))
    {
        perror("绑定失败");
       exit(-1);
    }
    printf("绑定创建\n");
    if((connfd=listen(listenfd,50))<0)
    {
        perror("监听失败");
        exit(-1);
    }
    printf("开始监听\n");
    FD_ZERO(&readfds);//初始化文件集
    FD_SET(listenfd,&readfds);//将需要监视的listenfd放入readfds集中
 
    while(1)//循环监听
    {
        int nread,n;
        tmpfds=readfds;//将监视集传递给临时的监视集中，防止后续的操作修改了监视集
        if(select(FD_SETSIZE,&tmpfds,NULL,NULL,NULL)<0)//设置监视，监视tmpfds内的子fd变化,发生变化的将会被保留在tmpfds中
          {
           perror("监视未成功");
            exit(-1);
         }
         
         for(fd1=0;fd1<FD_SETSIZE;fd1++)//循环找在最大范围内的fd1
        {
           if(FD_ISSET(fd1,&tmpfds))//查找是否fd1在tmpfds里面
            {
              if(fd1==listenfd)//判定fd1等于监听fd，即监听fd在监视过程中出现变化被发现
              {
                  addrlen=sizeof(cliaddr);
                 connfd=accept(listenfd,(struct sockaddr*)&cliaddr,&addrlen);//开始接收客户
                 FD_SET(connfd,&readfds);//将connfd加入监视集，监视接入的变化
                 printf("接入新的连接\n");
              }
              else
                  {
                      ioctl(fd1,FIONREAD,&nread);//测试在fd中还有nread个字符需要读取
                      if(nread==0)//如果需要读取的数据为0,则关闭检测出来的fd1，并且从监视中移除，例如当客户端crtl+c结束
                    {
                     close(fd1);
                     FD_CLR(fd1,&readfds);
                      printf("移除\n");
                    }
                      else//如果可读数据不为0，则读出来
                    {
                        int i,j;
                        char *p=NULL,*q=NULL;
												char src[4096]={};
												char filename[20]={};
	                      n=read(fd1,buf,nread);
	                      buf[n]='\0';
	                      p=buf;
                                              
                          if((strncmp(p,"-get",4)==0))
                          {  
                            // printf("客户下载文件>%s",q);这种方法获取文件名出错
									          //获取下载的文件名                   
												    for(j=0;j<(n-6);j++)
												    {
																src[j]=buf[5+j];
												    }
												    src[n-6]='\0';
												    q=src;
												    printf("客户下载文件>%s\n",q);

			    									if((fd2=open(q,O_RDONLY|O_CREAT))<0)
                                 perror("打开文件错误");
                             
                             while((m=read(fd2,buf,4096))>0)
                             {
                                 write(connfd,buf,m);
                                  
                             }
                             bzero(buf,sizeof(buf));
                             close(fd1);
                             close(fd2);
                             FD_CLR(fd1,&readfds);
                           
                          }
                           
                          if((strncmp(p,"-up",3)==0))
                          {  
									         //获取下载的文件名  			    
			    								 for(j=0;j<(n-5);j++)
                            {
                                src[j]=buf[4+j];
                            }
                            src[n-5]='\0';
                            q=src;
                           
                             printf("客户上传文件%s\n",q);
                             if((fd2=open(q,O_CREAT | O_WRONLY | O_APPEND ,0666))<0)
                             {
                                perror("打开文件写入失败");
                                  
                             }
                              
                             while((m=read(connfd,buf,128))>0)
                             { 
                                 printf("%s",buf);
                                 write(fd2,buf,m);
                             }
                             bzero(buf,sizeof(buf));
                             close(fd1);
                             close(fd2);
                             FD_CLR(fd1,&readfds);
                           
                          }
                           
                          if((strncmp(p,"-ls",3)==0))
                          {  
                           //  q=p+4;
									         //获取下载的文件名  
		 	     									for(j=0;j<(n-5);j++)
                            {
                                src[j]=buf[4+j];
                            }
                            src[n-5]='\0';
                            q=src;
			     
                             printf("客户查看文件……\n");
                             if((dr=opendir(q))==NULL)
                                 perror("打开目录失败");
			     
                             while((file=readdir(dr))!=NULL)
                             {
															   printf("%s    ",file->d_name);
														     write(connfd,file->d_name,sizeof(file->d_name));
                              }
                             close(fd1);
                             close(connfd);
                             closedir(dr);
                             FD_CLR(fd1,&readfds);
                           
                          }
                       
                      printf("从客户收取的信息:%s\n",buf);
                    }
 
 
                  }
 
            }//end if 0
             
          
        }//end for 0
 
     
    }//end while0
     
    exit(0);
}//end main



//-------client----/
#include <stdlib.h> 
#include <stdio.h> 
#include <errno.h> 
#include <string.h> 
#include <netdb.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 

  
#include <unistd.h>
#include <fcntl.h>
#include<sys/stat.h>

#define portnumber 3333
#define MAX_BUFFER_SIZE 1024

int main(int argc, char *argv[]) 
{ 
	int sockfd,fd2,m; 
	char buffer[1024]="",buf[1024]="";
	char finish[100]="ls finish"; 
	struct sockaddr_in server_addr; 
	struct hostent *host;

	int size,real_read;
	
 	int i,j;
                        char *p=NULL,*q=NULL;
                        char src[4096]={};
	int nread,n;
 /* 使用hostname查询host 名字 */
	if(argc!=2) 
	{ 
		fprintf(stderr,"Usage:%s hostname \a\n",argv[0]); 
		exit(1); 
	} 
	if((host=gethostbyname(argv[1]))==NULL) 
	{ 
		fprintf(stderr,"Gethostname error\n"); 
		exit(1); 
	} 
	/* 客户程序开始建立 sockfd描述符 */ 
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1) // AF_INET:Internet;SOCK_STREAM:TCP
	{ 
		fprintf(stderr,"Socket Error:%s\a\n",strerror(errno)); 
		exit(1); 
	} 

/* 客户程序填充服务端的资料 */ 
	bzero(&server_addr,sizeof(server_addr)); 			// 初始化,置0
	server_addr.sin_family=AF_INET;          				// IPV4
	server_addr.sin_port=htons(portnumber); 				 // (将本机器上的short数据转化为网络上的short数据)端口号
	server_addr.sin_addr=*((struct in_addr *)host->h_addr); // IP地址


/* 客户程序发起连接请求 */ 
	if(connect(sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr))==-1) 
	{ 
		fprintf(stderr,"Connect Error:%s\a\n",strerror(errno)); 
		exit(1); 
	} 
	/* 连接成功了 */ 
	printf("Please input char:\n");

	memset(buffer, 0, MAX_BUFFER_SIZE);
	size = read(0, buffer, MAX_BUFFER_SIZE);				//读取监听的数据

	write(sockfd,buffer,strlen(buffer)); 

	if((strncmp(buffer,"-get",4)==0)								//当客户端发出下载请求时，读取从服务器传来的数据
	{
	   real_read = read(sockfd, buffer, MAX_BUFFER_SIZE);//读取监听的数据
	   buffer[real_read] = '\0';
	   printf("%s", buffer);
	}

	if((strncmp(buffer,"-up",3)==0))								//当客户端发出上传的请求时，将需要发送的数据文件传送到服务器
        {
          		   for(j=0;j<(size-5);j++)
           		   {
                                src[j]=buffer[4+j];
                            }
                            src[size-5]='\0';
                            q=src;
                           // printf("客户下载文件>%s",q);
	   if((fd2=open(q,O_RDONLY|O_CREAT))<0)
                   perror("打开文件错误");
          
	  	 while((m=read(fd2,buffer,4096))>0)
          	 {
               		 write(sockfd,buffer,m);
          	 }
	   
	}
	if((strncmp(buffer,"-ls",3)==0))									//获取服务器目录
        {
	  
	   
        	  while((m=read(sockfd,buf,1024))>0)
         	 {
           	  printf("%s ",buf);
         	 }
	   
	   printf("\n");
        }
	/* 结束通讯 */ 
	close(sockfd); 
	exit(0); 
} 





