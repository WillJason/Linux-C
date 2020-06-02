#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

void my_func(int signum)
{
	printf("If you want to quit,please try SIGQUIT\n");
}

int main()
{
	sigset_t set,pendset;
	struct sigaction action1,action2;
	
	if(sigemptyset(&set)<0)
		perror("sigemptyset");
	
	if(sigaddset(&set,SIGQUIT)<0)
		perror("sigaddset");
	
	if(sigaddset(&set,SIGINT)<0)
		perror("sigaddset");
	
	if(sigprocmask(SIG_BLOCK,&set,NULL)<0)
		perror("sigprocmask");
	else
	{
		printf("blocked\n");
		sleep(5);
	}
	
	if(sigprocmask(SIG_UNBLOCK,&set,NULL)<0)
		perror("sigprocmask");
	else
		printf("unblock\n");
	
	while(1)
	{
		if(sigismember(&set,SIGINT))
		{
			sigemptyset(&action1.sa_mask);
			action1.sa_handler=my_func;
			sigaction(SIGINT,&action1,NULL);
		}
		else if(sigismember(&set,SIGQUIT))
		{
			sigemptyset(&action2.sa_mask);
			action2.sa_handler = SIG_DFL;
			sigaction(SIGTERM,&action2,NULL);
		}
	}
}

