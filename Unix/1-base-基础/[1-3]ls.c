/*
ls����ļ�Ҫʵ��
��1��apue.h�������Զ����һ��ͷ�ļ���������Unix/lLinuxϵͳ�Դ��ģ�������Ҫ��www.apuebook.com������Դ���롣err_qiut��err_sysҲ������apue��ѹ���ļ����error.c�У��������Զ���Ĵ���������

��2��������֮��src.3e.tar.gz��ѹ��tar -zvxf src.3e.tar.gz

��3�������ѹ�ļ� ��cd apue.3e

��4��make ?���û��������������lib�ļ����޷�����libapue.a

��5����apue.hͷ�ļ���libapue.a��̬���error.c�ļ����Ƶ�����Ĭ�ϵ�·���¡�������root�û��²���Ȩ��

$ cp ./include/apue.h /usr/include

$ cp ./lib/libapue.a /usr/lib

$ cp ./lib/error.c /usr/lib

��6��make

��7����ls(1)�����а���ͷ�ļ�#include "error.c"

��8��gcc ��������
*/
#include <apue.h>
#include <dirent.h>

int main (int argc,char *argv[])
{
		DIR  *dp;
		struct dirent  *dirp;
		
		if(argc!=2)
			err_quit("usage:ls directory_name");
			
		if(dp=opendir(argv[1])==NULL)
			err_sys("can't open %s",argv[1]);
			
		while((dirp=readddir(dp))!=NULL)
			printf("%s\n",dirp->d.name);
			
		closedir(dp);
		exit(0);
}




