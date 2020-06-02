#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc,char **argv)
{
		int fd;
		if(argc<2)
		{
				puts("please input the open file pathname!\n");
				exit(1);
		}
		
		if((fd=open(argv[1],O_CREAT|O_RDWR,0755))<0)
		{
				perror("open file failure!\n");
				exit(1);
		}
		else
		{
				printf("open file %d success!\n",fd);
		}
		close(fd);
		exit(0);
}



