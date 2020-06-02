/*程序取其命令行参数，然后针对每一个命令行参数打印其文件类型*/
#include <apue.h>


int main(int argc,char *argv[])
{
		int i;
		struct stat buf;
		char *ptr;
		
		for(i=1;i<argc;i++)
		{
				printf("%s: ",argv[i]);
				/*
				我们特地使用了lstat函数而不是stat函数以便检测符号链接。如果哦使用stat函数，则不会观察到符号链接
				*/
				if(lstat(argv[i],&buf)<0)
				{
					err_ret("lstat error");
					continue;
				}
				if(S_ISREG(buf.st_mode))
					ptr="regular";
				else if(S_ISDIR(buf.st_mode)) 
					ptr="directory";	
				else if(S_ISCHR(buf.st_mode)) 
					ptr="character special";
				else if(S_ISBLK(buf.st_mode)) 
					ptr="block special";
				else if(S_ISFIFO(buf.st_mode)) 
					ptr="fifo";
				else if(S_ISLNK(buf.st_mode)) 
					ptr="symbollic link";
				else if(S_ISSOCK(buf.st_mode)) 
					ptr="socket";
				else
					ptr="**  unknown mode**";
				printf("%s\n",ptr);
		}
		exit(0);
}