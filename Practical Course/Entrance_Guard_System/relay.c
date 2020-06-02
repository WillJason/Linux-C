#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void relay_set(char *relay_dev,int state)
{
	int fd;
	
	fd = open(relay_dev, O_RDWR);

	ioctl(fd,state,0);
	
	close(fd);
}
