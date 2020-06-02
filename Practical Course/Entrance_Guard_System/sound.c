#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void buzzer_set(char *buzzer_dev , int state)
{
	int fd;
	
	fd = open(buzzer_dev, O_RDWR);
	if(fd<0)
		printf("open %s failed\n",buzzer_dev); 
	ioctl(fd,state,0);

	close(fd);
}
