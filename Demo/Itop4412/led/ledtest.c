#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

void main(int argc, char* argv[])
{
	char *max485_ctl = "/dev/leds";
	
	int fd, ret;

	if(argc < 2)
	{
		printf("usage: ./ledtest 1 1\n");

		exit(1);
	}
	
	//O_NOCTTY 如果p a t h n a m e指的是终端设备，则不将此设备分配作为此进程的控制终端。
	//O_NDELAY使I/O变成非搁置模式。设立O_NDELAY会使I/O函式马上回传0，但是又衍生出一个问题，
	//因为读取到档案结尾时所回传的也是0，这样无法得知是哪中情况；因此，O_NONBLOCK就产生出来，它在读取不到数据时会回传-1，并且设置errno为EAGAIN
	if((fd = open("/dev/leds", O_RDWR|O_NOCTTY|O_NDELAY))<0)
        {
                printf("Open %s faild\n", max485_ctl);
                //close(fd1);

                exit(1);
        }

	ret = ioctl(fd, atoi(argv[1]), atoi(argv[2]));//atoi (表示 ascii to integer)是把字符串转换成整型数的一个函数

	return ;
}
