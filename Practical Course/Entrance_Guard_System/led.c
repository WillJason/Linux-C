#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void led_set(char *led_dev,int cmd,int lednum)
{
	int fd;
	
	fd = open(led_dev, O_RDWR);
	if(lednum>1 || cmd>1){
		printf("led arg is err\n");		
	}

	ioctl(fd,cmd,lednum);
	
	close(fd);
}
