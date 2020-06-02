#include <stdio.h> 
#include <stdlib.h> 

#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 


void creat_file(char *filename)
{
		/*创建的文件具有什么样的属性？*/
		if(creat(filename,0755)<0)
		{
				printf("create file %s failure!\n",filename);
				exit(EXIT_FAILURE);
		}
		else
		{
				printf("create file %s success!\n",filename);
		}
}

int main(int agrc,char **argv)
{
		int i;
		if(argc<2)
		{
				perror("you haven't input the filename,please try again!\n");
				exit(EXIT_FAILURE);
		}
		
		for(i=1;i<argc;i++)
		{
			craet_file(argv[i]);
		}
		exit(EXIT_SUCCESS);
}
