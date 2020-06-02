#include <apue.h>

static void sig_int(int);

int main()
{
	sigset_t netmask,oldmask,waitmask;
	
	pr_mask("program start: ");
	
	if(signal(SIGINT,sig_int) == SIGERR)
		err_sys("signal(SIGINT) error");
	sigemptyset(&waitmask);
	sigaddset(&waitmask,SIGUSR1);
	sigemptyset(&newmask);
	sigaddset(&waitmask,SIGINT);
	
	/*
	Block SIGINT and save current signal mask.
	*/
	if(sigprocmask(SIG_BLOCK,&newmask,&oldmask) < 0)
		err_sys("SIG_BLOCK error");
		
	/*
	Critical region of code.
	*/
	pr_mask("in critical region: ");
	/*
	Pause,allowing all signals except SIGUSR1.
	*/
	if(sigprocmask(SIG_SETMASK,&oldmask,NULL) < 0)
		err_sys("SIG_SETMASK error");
	/*
	And continue processing ...
	*/
	pr_mask("program exit: ");
	
	exit(0);
}

static void sig_int(int signo)
{
		pr_mask("\nin sig_int: ");
}
