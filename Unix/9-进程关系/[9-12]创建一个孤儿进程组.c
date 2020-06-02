/*
在fork之后：
子进程睡眠5秒，这是一种让子进程在父进程终止之前运行的权宜之计。
子进程为挂断信号（SIGHUP）建立信号处理程序。这样就能观察到SIGHUP信号是否已经发送给子进程。
子进程用KILL函数向其自身发送停止信号(SIGSTP)。这将停止子进程，类似于终端挂起字符停止一个前台作业。
当父进程终止时，该子进程成为孤儿进程，所以父进程ID成为1，也就是init进程ID。
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
在处理了挂断信号后，子进程继续。对挂断信号的系统默认动作是终止该进程，
为此必须提供一个信号处理程序以捕捉该信号。
*/
