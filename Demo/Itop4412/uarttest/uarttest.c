#include <stdio.h>

#include <string.h>

#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <termios.h>
#include <errno.h>

int set_opt(int,int,int,char,int);
void leds_control(int);

void main(int argc, char* argv[])
{
	int fd, read_num = 0, flag = 0;
	//char *uart3 = "/dev/ttySAC3";
	
	
	unsigned char buffer[1024],buffer_test[1024]; 
	memset(buffer, 0, 1024);
	memset(buffer_test, 0, 1024);
	
	if(argc < 2)
	{
		printf("usage: ./uarttest /dev/ttySAC3\n");

		return;
	}
	
	char *uart_out = "please input\r\n";
	char *head_out = " output is \r\n";
	
	if((fd = open(argv[1], O_RDWR|O_NOCTTY|O_NDELAY))<0)
	{	
		printf("open %s is failed\n", argv[1]);
		
		return;
	}
	else{
	set_opt(fd, 115200, 8, 'N', 1); 

		write(fd,uart_out, strlen(uart_out));
	
	while(1){
		memset(buffer, 0, 1024);
		
		read_num = read(fd, buffer, 100);
		
		if(read_num>0){
		write(fd,"\r\n",2);
		write(fd,buffer,strlen(buffer));
		
	/*	flag++;
		buffer_test[flag] = buffer[0];
		if(buffer_test[flag]=='\n'){
		write(fd,"\r\n",2);
		write(fd,buffer_test,strlen(buffer_test));
		flag = 0;
		}*/
		write(fd,"\r\n",2);	
		
		memset(buffer,0,read_num);
		sprintf(buffer,"\n  output num is %d\r\n",read_num);
		
		write(fd,buffer,strlen(buffer));
		}
		}
	}
}



int set_opt(int fd,int nSpeed, int nBits, char nEvent, int nStop)
{
	struct termios newtio,oldtio;
	if  ( tcgetattr( fd,&oldtio)  !=  0) { 
		perror("SetupSerial 1");
		return -1;
	}
	bzero( &newtio, sizeof( newtio ) );
	newtio.c_cflag  |=  CLOCAL | CREAD;
	newtio.c_cflag &= ~CSIZE;

	switch( nBits )
	{
	case 7:
		newtio.c_cflag |= CS7;
		break;
	case 8:
		newtio.c_cflag |= CS8;
		break;
	}

	switch( nEvent )
	{
	case 'O':
		newtio.c_cflag |= PARENB;
		newtio.c_cflag |= PARODD;
		newtio.c_iflag |= (INPCK | ISTRIP);
		break;
	case 'E': 
		newtio.c_iflag |= (INPCK | ISTRIP);
		newtio.c_cflag |= PARENB;
		newtio.c_cflag &= ~PARODD;
		break;
	case 'N':  
		newtio.c_cflag &= ~PARENB;
		break;
	}

	switch( nSpeed )
	{
	case 2400:
		cfsetispeed(&newtio, B2400);
		cfsetospeed(&newtio, B2400);
		break;
	case 4800:
		cfsetispeed(&newtio, B4800);
		cfsetospeed(&newtio, B4800);
		break;
	case 9600:
		cfsetispeed(&newtio, B9600);
		cfsetospeed(&newtio, B9600);
		break;
	case 115200:
		cfsetispeed(&newtio, B115200);
		cfsetospeed(&newtio, B115200);
		break;
	case 460800:
		cfsetispeed(&newtio, B460800);
		cfsetospeed(&newtio, B460800);
		break;
	case 921600:
		printf("B921600\n");
		cfsetispeed(&newtio, B921600);
                cfsetospeed(&newtio, B921600);
		break;
	default:
		cfsetispeed(&newtio, B9600);
		cfsetospeed(&newtio, B9600);
		break;
	}
	if( nStop == 1 )
		newtio.c_cflag &=  ~CSTOPB;
	else if ( nStop == 2 )
	newtio.c_cflag |=  CSTOPB;
	newtio.c_cc[VTIME]  = 0;
	newtio.c_cc[VMIN] = 0;
	tcflush(fd,TCIFLUSH);
	if((tcsetattr(fd,TCSANOW,&newtio))!=0)
	{
		perror("com set error");
		return -1;
	}
//	printf("set done!\n\r");
	return 0;
}


