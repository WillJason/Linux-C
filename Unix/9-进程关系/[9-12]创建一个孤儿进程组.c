/*
��fork֮��
�ӽ���˯��5�룬����һ�����ӽ����ڸ�������ֹ֮ǰ���е�Ȩ��֮�ơ�
�ӽ���Ϊ�Ҷ��źţ�SIGHUP�������źŴ�������������ܹ۲쵽SIGHUP�ź��Ƿ��Ѿ����͸��ӽ��̡�
�ӽ�����KILL��������������ֹͣ�ź�(SIGSTP)���⽫ֹͣ�ӽ��̣��������ն˹����ַ�ֹͣһ��ǰ̨��ҵ��
����������ֹʱ�����ӽ��̳�Ϊ�¶����̣����Ը�����ID��Ϊ1��Ҳ����init����ID��
*/
#include <apue.h>
#include <errno.h>

static void
sig_hup(int signo)
{
		printf("SIGHUP received,pid = %1d\n",(long)getpid());
}

static void pr_ids(char *name)
{
	printf("%s:pid = %1d,ppid = %1d,pgrp = %1d,tpgrp = %1d\n",name,(long)getpid(),(long)getppid(),(long)getpgrp(),(long)tcgetpgrp(STDIN_FILENO));
	fflush(stdout);
}

int main(void)
{
	char  c;
	pid_t  pid;
	
	pr_ids("parent");
	if((pid = fork()) < 0)
	{
		err_sys("fork error");
	}
	else if(pid > 0)/* parent*/
	{
		sleep(5);
	}
	else
	{
		pr_ids("child");
		signal(SIGHUP,sig_hup);
		kill(getpid(),SIGTSTP);
		pr_ids("child");
		if(read(STDIN_FILENO,&c,1) != 1)
			printf("read error %d on controlling TTY\n",errno);
	}
	exit(0);
}
/*
�ڴ����˹Ҷ��źź��ӽ��̼������ԹҶ��źŵ�ϵͳĬ�϶�������ֹ�ý��̣�
Ϊ�˱����ṩһ���źŴ�������Բ�׽���źš�
*/
