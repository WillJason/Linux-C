#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h> 


#define BUFFER_SIZE 1024

int main(int argc,char **argv)
{
		int from_fd,to_fd;
		int bytes_read,bytes_write;
		char buffer[BUFFER_SIZE];  
		char *ptr; 
		
		if(argc!=3)
		{
				fprintf(stderr,"Usage:%s fromfile tofile/n/a",argv[0]);
				exit(1);
		}
		
		/* ��Դ�ļ� */
		if((from_fd=open(argv[1],O_RDONLY))==-1)
		{
				fprintf(stderr,"Open %s Error:%s/n",argv[1],strerror(errno));
				exit(1);
		}
		
		/* ����Ŀ���ļ� */ 
		if((to_fd=open(argv[2],O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR))==-1)
		{
				fprintf(stderr,"Open %s Error:%s/n",argv[1],strerror(errno));
				exit(1);
		}
		
		/* ���´�����һ������Ŀ����ļ��Ĵ��� */
		while(bytes_read=read(froom_fd,buffer,BUFFER_SIZE))
		{
			/* һ�������Ĵ������� */ 
			if((bytes_read==-1)&&(errno!=EINTR)) break; 
			else if(bytes_read>0) 
			{
					ptr=buffer; 
					while(bytes_write=write(to_fd,ptr,bytes_read)) 
					{
							/* һ�������������� */ 
							if((bytes_write==-1)&&(errno!=EINTR))break; 
								/* д�������ж����ֽ� */
							else if(bytes_write==bytes_read) break; 
								/* ֻд��һ����,����д */ 
							else if(bytes_write>0)
							{
								ptr+=bytes_write;
								bytes_read-=bytes_write;
							}
					}
					/* д��ʱ�������������� */ 
					if(bytes_writes==-1)break;
			}		
		}
		close(from_fd); 
		close(to_fd); 
		exit(0); 
}