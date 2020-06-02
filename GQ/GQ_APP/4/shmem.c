#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define PERM S_IRUSR|S_IWUSR
/* 共享内存 */

int main(int argc,char **argv) 
{ 
	int shmid; 
	char *p_addr,*c_addr; 
	
	if(argc!=2) 
	{ 
		fprintf(stderr,"Usage:%s\n\a",argv[0]); 
		exit(1); 
	}

	/* 创建共享内存 */	
	if((shmid=shmget(IPC_PRIVATE,1024,PERM))==-1) 
	{ 
		fprintf(stderr,"Create Share Memory Error:%s\n\a",strerror(errno)); 
		exit(1); 
	} 

	/* 创建子进程 */
	if(fork()) // 父进程写
	{ 
		p_addr=shmat(shmid,0,0); 
		memset(p_addr,'\0',1024); 
		strncpy(p_addr,argv[1],1024);
		wait(NULL); // 释放资源,不关心终止状态
		exit(0); 
	} 
	else       // 子进程读
	{ 
		sleep(1); // 暂停1秒		
		c_addr=shmat(shmid,0,0); 
		printf("Client get %p\n",c_addr); 
		exit(0); 
	} 
} 
