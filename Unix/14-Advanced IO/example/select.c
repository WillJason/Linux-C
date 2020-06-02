//������ʾ�����룺
//�����Ƿ�����TCPģ�ͣ����ö�·���õ�select����ʵ����ѭ���ļ��������ܿͻ��˵Ĺ��ܣ�����Ҳ�������ϴ����صĹ���*/

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

    fd_set  readfds,tmpfds;//���ڼ�ؼ���
    socklen_t addrlen;
    DIR *dr;
    struct dirent *file;
 
    printf("��������Ҫ�趨�ķ���������:");
    scanf("%s",user);
     
    printf("��������Ҫ�趨�ķ������˿�:");
    scanf("%d",&port);
    getchar();
 
    if((listenfd=socket(AF_INET,SOCK_STREAM,0))<0)
    {
        perror("����ʧ��");
       exit(-1);
    }
     
     
 
    /*��ʼ�趨�������Ĳ�����ַ����,IP,PORT*/
    memset(&seraddr,0,sizeof(seraddr));//���������ĳ�ֵ�ռ���գ���ֹת��������Ӱ��
    seraddr.sin_family=AF_INET;
    seraddr.sin_port=htons(port);//���õ��ı��ض˿�ת��Ϊ�����ֽ���
    seraddr.sin_addr.s_addr=htonl(INADDR_ANY);//���õ���ip��ַ�ַ���ת��Ϊ�����ֽ����ip��ַ��ֵ
 
    if((bind(listenfd,(struct sockaddr*)&seraddr,sizeof(seraddr))<0))
    {
        perror("��ʧ��");
       exit(-1);
    }
    printf("�󶨴���\n");
    if((connfd=listen(listenfd,50))<0)
    {
        perror("����ʧ��");
        exit(-1);
    }
    printf("��ʼ����\n");
    FD_ZERO(&readfds);//��ʼ���ļ���
    FD_SET(listenfd,&readfds);//����Ҫ���ӵ�listenfd����readfds����
 
    while(1)//ѭ������
    {
        int nread,n;
        tmpfds=readfds;//�����Ӽ����ݸ���ʱ�ļ��Ӽ��У���ֹ�����Ĳ����޸��˼��Ӽ�
        if(select(FD_SETSIZE,&tmpfds,NULL,NULL,NULL)<0)//���ü��ӣ�����tmpfds�ڵ���fd�仯,�����仯�Ľ��ᱻ������tmpfds��
          {
           perror("����δ�ɹ�");
            exit(-1);
         }
         
         for(fd1=0;fd1<FD_SETSIZE;fd1++)//ѭ���������Χ�ڵ�fd1
        {
           if(FD_ISSET(fd1,&tmpfds))//�����Ƿ�fd1��tmpfds����
            {
              if(fd1==listenfd)//�ж�fd1���ڼ���fd��������fd�ڼ��ӹ����г��ֱ仯������
              {
                  addrlen=sizeof(cliaddr);
                 connfd=accept(listenfd,(struct sockaddr*)&cliaddr,&addrlen);//��ʼ���տͻ�
                 FD_SET(connfd,&readfds);//��connfd������Ӽ������ӽ���ı仯
                 printf("�����µ�����\n");
              }
              else
                  {
                      ioctl(fd1,FIONREAD,&nread);//������fd�л���nread���ַ���Ҫ��ȡ
                      if(nread==0)//�����Ҫ��ȡ������Ϊ0,��رռ�������fd1�����ҴӼ������Ƴ������統�ͻ���crtl+c����
                    {
                     close(fd1);
                     FD_CLR(fd1,&readfds);
                      printf("�Ƴ�\n");
                    }
                      else//����ɶ����ݲ�Ϊ0���������
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
                            // printf("�ͻ������ļ�>%s",q);���ַ�����ȡ�ļ�������
									          //��ȡ���ص��ļ���                   
												    for(j=0;j<(n-6);j++)
												    {
																src[j]=buf[5+j];
												    }
												    src[n-6]='\0';
												    q=src;
												    printf("�ͻ������ļ�>%s\n",q);

			    									if((fd2=open(q,O_RDONLY|O_CREAT))<0)
                                 perror("���ļ�����");
                             
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
									         //��ȡ���ص��ļ���  			    
			    								 for(j=0;j<(n-5);j++)
                            {
                                src[j]=buf[4+j];
                            }
                            src[n-5]='\0';
                            q=src;
                           
                             printf("�ͻ��ϴ��ļ�%s\n",q);
                             if((fd2=open(q,O_CREAT | O_WRONLY | O_APPEND ,0666))<0)
                             {
                                perror("���ļ�д��ʧ��");
                                  
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
									         //��ȡ���ص��ļ���  
		 	     									for(j=0;j<(n-5);j++)
                            {
                                src[j]=buf[4+j];
                            }
                            src[n-5]='\0';
                            q=src;
			     
                             printf("�ͻ��鿴�ļ�����\n");
                             if((dr=opendir(q))==NULL)
                                 perror("��Ŀ¼ʧ��");
			     
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
                       
                      printf("�ӿͻ���ȡ����Ϣ:%s\n",buf);
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
 /* ʹ��hostname��ѯhost ���� */
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
	/* �ͻ�����ʼ���� sockfd������ */ 
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1) // AF_INET:Internet;SOCK_STREAM:TCP
	{ 
		fprintf(stderr,"Socket Error:%s\a\n",strerror(errno)); 
		exit(1); 
	} 

/* �ͻ�����������˵����� */ 
	bzero(&server_addr,sizeof(server_addr)); 			// ��ʼ��,��0
	server_addr.sin_family=AF_INET;          				// IPV4
	server_addr.sin_port=htons(portnumber); 				 // (���������ϵ�short����ת��Ϊ�����ϵ�short����)�˿ں�
	server_addr.sin_addr=*((struct in_addr *)host->h_addr); // IP��ַ


/* �ͻ��������������� */ 
	if(connect(sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr))==-1) 
	{ 
		fprintf(stderr,"Connect Error:%s\a\n",strerror(errno)); 
		exit(1); 
	} 
	/* ���ӳɹ��� */ 
	printf("Please input char:\n");

	memset(buffer, 0, MAX_BUFFER_SIZE);
	size = read(0, buffer, MAX_BUFFER_SIZE);				//��ȡ����������

	write(sockfd,buffer,strlen(buffer)); 

	if((strncmp(buffer,"-get",4)==0)								//���ͻ��˷�����������ʱ����ȡ�ӷ���������������
	{
	   real_read = read(sockfd, buffer, MAX_BUFFER_SIZE);//��ȡ����������
	   buffer[real_read] = '\0';
	   printf("%s", buffer);
	}

	if((strncmp(buffer,"-up",3)==0))								//���ͻ��˷����ϴ�������ʱ������Ҫ���͵������ļ����͵�������
        {
          		   for(j=0;j<(size-5);j++)
           		   {
                                src[j]=buffer[4+j];
                            }
                            src[size-5]='\0';
                            q=src;
                           // printf("�ͻ������ļ�>%s",q);
	   if((fd2=open(q,O_RDONLY|O_CREAT))<0)
                   perror("���ļ�����");
          
	  	 while((m=read(fd2,buffer,4096))>0)
          	 {
               		 write(sockfd,buffer,m);
          	 }
	   
	}
	if((strncmp(buffer,"-ls",3)==0))									//��ȡ������Ŀ¼
        {
	  
	   
        	  while((m=read(sockfd,buf,1024))>0)
         	 {
           	  printf("%s ",buf);
         	 }
	   
	   printf("\n");
        }
	/* ����ͨѶ */ 
	close(sockfd); 
	exit(0); 
} 





