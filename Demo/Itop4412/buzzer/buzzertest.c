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
	char *max485_ctl = "/dev/buzzer_ctl";
	
	int fd, ret;

	if(argc < 2)
	{
		printf("usage: ./buzzertest 1 1\n");

		exit(1);
	}

	if((fd = open(max485_ctl, O_RDWR|O_NOCTTY|O_NDELAY))<0)
        {
                printf("Open %s faild\n", max485_ctl);
                //close(fd1);

                exit(1);
        }

	ret = ioctl(fd, atoi(argv[1]), atoi(argv[2]));

	return ;
}
