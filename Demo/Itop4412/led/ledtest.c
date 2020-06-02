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
	
	//O_NOCTTY ���p a t h n a m eָ�����ն��豸���򲻽����豸������Ϊ�˽��̵Ŀ����նˡ�
	//O_NDELAYʹI/O��ɷǸ���ģʽ������O_NDELAY��ʹI/O��ʽ���ϻش�0��������������һ�����⣬
	//��Ϊ��ȡ��������βʱ���ش���Ҳ��0�������޷���֪�������������ˣ�O_NONBLOCK�Ͳ������������ڶ�ȡ��������ʱ��ش�-1����������errnoΪEAGAIN
	if((fd = open("/dev/leds", O_RDWR|O_NOCTTY|O_NDELAY))<0)
        {
                printf("Open %s faild\n", max485_ctl);
                //close(fd1);

                exit(1);
        }

	ret = ioctl(fd, atoi(argv[1]), atoi(argv[2]));//atoi (��ʾ ascii to integer)�ǰ��ַ���ת������������һ������

	return ;
}
