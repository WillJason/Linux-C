/*
�ͻ���α����
	pid = fork();
	if(pid){
		while(1){
			if(���յ���������)
				ͨ�����ڽ����ݷ���
		}
	}
	else if(pid){
		while(1){
			if(���յ���������)
				ͨ�����ڽ������ͳ�
		}
	}

	IP����ͨ���������ã�����port�˿ں�Ҳ��������
	port�˿ںŵ�һ�α�ռ�ã�����ȹرշ���ˣ��ͻ��˻���ռ�ö˿ں�
	�˿ںŵ�ȡֵ��Χ��0-65535,�˿ں�0-1024��
	�˿ں�����Ҫ��ͬ��
	
ʹ�ã�
./client ������IP 12345	
*/
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>

extern int net_init(char *server_ip,unsigned short portvalue);
extern int port_config(int,int,int,char,int);
extern int uart_start(char *uart,char *uart_buffer);

int main(int argc, char **argv)
{
	int Net_fd;
	int recbyte,nByte;
	char buffer[1024] = {0};
	pid_t pid;
	unsigned short portnum = 0x8888;
	int Uart_fd,UartWr_Bytes,i=10;
	char *uart3 = "/dev/ttySAC3";
	char *uart_buffer = "hello world!\r\n";
	
	printf("Hello,welcome to client!\r\n");

	if(argc == 3){
		portnum = atoi(argv[2]);
		printf("portnum is 0x%x !\n",portnum);
	}
	
	if(argc < 2){
		printf("usage: echo ip\n");
		return -1;
	}
	if((Net_fd = net_init(argv[1],portnum))==0){
		return -1;
	}
	if((Uart_fd = uart_start(uart3,uart_buffer))==-1){
		return -1;
	}
	
	pid = fork();
	if(pid == -1){
		printf("fork failed\n");
		return 1;
	}
	else if(pid){
		while(1){
			if(-1 == (recbyte = read(Net_fd, buffer, 1024))){
				printf("read data fail !\r\n");
				return -1;
			}
			buffer[recbyte]='\0';
			printf("%s\r\n",buffer);
			
			if(strlen(buffer)>0){
				UartWr_Bytes = write(Uart_fd,buffer, strlen(buffer));
				memset(buffer, 0, 1024);
			}
			usleep(50);
		}
	}
	else{
		while(1){
			while((nByte = read(Uart_fd, buffer, 512))>0){
				write(Uart_fd,buffer,nByte);
				printf("read uart1 data %s!\n",buffer);
				
				buffer[nByte]='\0';
				printf("%s\r\n",buffer);
			
				send(Net_fd, buffer, nByte, 0);
				nByte = 0;	
			}
			usleep(50);
		}
	}
	
	close(Net_fd);
	close(Uart_fd);
	
	return 0;
}