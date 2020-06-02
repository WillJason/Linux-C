/*
传统网关:将一种协议转换为另一种协议，实现数据转发而不做控制。
1.协议转换 2.数据转发 3.不做控制
智能物联网网关：实现数据处理
zigbee协议中 指令头、指令尾无关信息不要，只发送有效信息

智能物联网网关实现的框架：
虚拟机ubuntu<-->以太网<-->网关设备(开发板)<-->串口<-->PC串口助手

以太网+串口组合==>硬件选择
涉及知识点：
网络通讯+串口通信+多进程.fork
伪代码：
开发板端程序逻辑
pid = fork();
if(pid){
	while(1){
		if(接收到网口数据){
			通过串口将数据发出
		}
	}
}
else if(pid){
	while(1){
		if(接收到串口数据){
			通过网口将数据发送出
		}
	}
}

Ubuntu端程序逻辑
pid = fork();
if(pid){
	while(1){
		if(接收到网口数据){
			打印
		}
	}
}
else if(pid){
	while(1){
		通过网口发送数据到开发板
	}
}

服务器端伪代码：打印。通过网口将数据发送到开发板。delay(1s)

使用：
./ubuntu_server 12345
*/
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

int main(int argc, char **argv)
{
	pid_t pid;
	int sfp, nfp, num = 0;
	struct sockaddr_in s_add,c_add;
	int sin_size,recbyte;
	unsigned short portnum=0x8888;
	char buffer[100] = {0};  
	
	printf("Hello,welcome to my server !\r\n");
	
	if(argc == 2){
		portnum = atoi(argv[1]);
		printf("portnum is 0x%x !\n",portnum);
	}
	
	sfp = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == sfp){
		printf("socket fail ! \r\n");
		return -1;
	}
	
	/*
			setsockopt()函数，用于任意类型、任意状态套接口的设置选项值。尽管在不同协议层上存在
	选项，但本函数仅定义了最高的“套接口”层次上的选项。
			缺省条件下，一个套接口不能与一个已在使用中的本地地址捆绑（参见bind()）。但有时会需
	要“重用”地址。因为每一个连接都由本地地址和远端地址的组合唯一确定，所以只要远端地址不
	同，两个套接口与一个地址捆绑并无大碍。为了通知套接口实现不要因为一个地址已被一个套接口
	使用就不让它与另一个套接口捆绑，应用程序可在bind()调用前先设置SO_REUSEADDR选项。请注意
	仅在bind()调用时该选项才被解释；故此无需（但也无害）将一个不会共用地址的套接口设置该选
	项，或者在bind()对这个或其他套接口无影响情况下设置或清除这一选项。
	*/
	int val = 1;  
	setsockopt(sfp,SOL_SOCKET,SO_REUSEADDR,(char *)&val,sizeof(val));
	
	printf("socket ok !\r\n");

	bzero(&s_add,sizeof(struct sockaddr_in));
	s_add.sin_family=AF_INET;
	s_add.sin_addr.s_addr=htonl(INADDR_ANY);
	s_add.sin_port=htons(portnum);

	if(-1 == bind(sfp,(struct sockaddr *)(&s_add), sizeof(struct sockaddr))){
		printf("bind fail !\r\n");
		return -1;
	}

	printf("bind ok !\r\n");

	if(-1 == listen(sfp,5)){
		printf("listen fail !\r\n");
		return -1;
	}
	printf("listen ok\r\n");

	sin_size = sizeof(struct sockaddr_in);
	nfp = accept(sfp, (struct sockaddr *)(&c_add), &sin_size);
	if(-1 == nfp){
		printf("accept fail !\r\n");
		return -1;
	}

	printf("accept ok!\r\nServer start get connect from %#x : %#x\r\n",ntohl(c_add.sin_addr.s_addr), ntohs(c_add.sin_port));
	
	pid = fork();
	if(pid == -1){
		printf("fork failed\n");
		return 1;
	}
	else if(pid){
		while(1){
			scanf("%s",buffer);
			send(nfp, buffer, strlen(buffer), 0);
			usleep(5);
		}
	}
	else{
		while(1){
			if(-1 == (recbyte = read(nfp, buffer, 1024))){
				printf("read data fail !\r\n");
				return -1;
			}
			if(recbyte > 0){
				buffer[recbyte]='\0';
				printf("socket data is %s!\n",buffer);			
			}		
			memset(buffer, 0, recbyte);	
			usleep(5);
		}
	}
	
over:
	close(nfp);
	close(sfp);
	return 0;			
}
