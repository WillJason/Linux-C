/*
��ͳ����:��һ��Э��ת��Ϊ��һ��Э�飬ʵ������ת�����������ơ�
1.Э��ת�� 2.����ת�� 3.��������
�������������أ�ʵ�����ݴ���
zigbeeЭ���� ָ��ͷ��ָ��β�޹���Ϣ��Ҫ��ֻ������Ч��Ϣ

��������������ʵ�ֵĿ�ܣ�
�����ubuntu<-->��̫��<-->�����豸(������)<-->����<-->PC��������

��̫��+�������==>Ӳ��ѡ��
�漰֪ʶ�㣺
����ͨѶ+����ͨ��+�����.fork
α���룺
������˳����߼�
pid = fork();
if(pid){
	while(1){
		if(���յ���������){
			ͨ�����ڽ����ݷ���
		}
	}
}
else if(pid){
	while(1){
		if(���յ���������){
			ͨ�����ڽ����ݷ��ͳ�
		}
	}
}

Ubuntu�˳����߼�
pid = fork();
if(pid){
	while(1){
		if(���յ���������){
			��ӡ
		}
	}
}
else if(pid){
	while(1){
		ͨ�����ڷ������ݵ�������
	}
}

��������α���룺��ӡ��ͨ�����ڽ����ݷ��͵������塣delay(1s)

ʹ�ã�
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
			setsockopt()�����������������͡�����״̬�׽ӿڵ�����ѡ��ֵ�������ڲ�ͬЭ����ϴ���
	ѡ�������������������ߵġ��׽ӿڡ�����ϵ�ѡ�
			ȱʡ�����£�һ���׽ӿڲ�����һ������ʹ���еı��ص�ַ���󣨲μ�bind()��������ʱ����
	Ҫ�����á���ַ����Ϊÿһ�����Ӷ��ɱ��ص�ַ��Զ�˵�ַ�����Ψһȷ��������ֻҪԶ�˵�ַ��
	ͬ�������׽ӿ���һ����ַ�����޴󰭡�Ϊ��֪ͨ�׽ӿ�ʵ�ֲ�Ҫ��Ϊһ����ַ�ѱ�һ���׽ӿ�
	ʹ�þͲ���������һ���׽ӿ�����Ӧ�ó������bind()����ǰ������SO_REUSEADDRѡ���ע��
	����bind()����ʱ��ѡ��ű����ͣ��ʴ����裨��Ҳ�޺�����һ�����Ṳ�õ�ַ���׽ӿ����ø�ѡ
	�������bind()������������׽ӿ���Ӱ����������û������һѡ�
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
