/*
在修改文件描述符标志或文件状态标志时必须谨慎，先要获得现在的标志值，
然后按照期望修改它，最后设置新标志值。不能只是执行F_SETFD或F_SERFL命令，这样会关闭以前设置的标志位。
*/
#include <apue.h>
#include <fcntl.h>

int set_fl(int fd,int flags)
{
		int val;
		
		if((val=fcntl(fd,F_GETFL,0))<0)//通过fcntl可以改变已打开的文件性质。fcntl针对描述符提供控制。
			err_sys("fcntl F_GETFL error ");
		
		val |=flags;
		
		if((val=fcntl(fd,F_SETFL,0))<0)
			err_sys("fcntl F_SETFL error ");
}