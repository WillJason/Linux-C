/*
���޸��ļ���������־���ļ�״̬��־ʱ�����������Ҫ������ڵı�־ֵ��
Ȼ���������޸�������������±�־ֵ������ֻ��ִ��F_SETFD��F_SERFL���������ر���ǰ���õı�־λ��
*/
#include <apue.h>
#include <fcntl.h>

int set_fl(int fd,int flags)
{
		int val;
		
		if((val=fcntl(fd,F_GETFL,0))<0)//ͨ��fcntl���Ըı��Ѵ򿪵��ļ����ʡ�fcntl����������ṩ���ơ�
			err_sys("fcntl F_GETFL error ");
		
		val |=flags;
		
		if((val=fcntl(fd,F_SETFL,0))<0)
			err_sys("fcntl F_SETFL error ");
}