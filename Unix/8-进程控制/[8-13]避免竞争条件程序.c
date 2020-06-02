#include <apue.h>

static void charatatime(char *);

int main(void)
{
	pid_t pid;
	
	TELL_WAIT();  													 //+
	
	if((pid =fork()) < 0)
	{
			err_sys("fork error");
	}
	else if(pid == 0)
	{
			WAIT_PARENT();    /*parent goes */   //+
			charatatime("output from child");
	}
	else
	{
			charatatime("output from parent");
			TELL_CHILD();												 //+
	}
	exit(0);
}


static void charatatime(char *str)
{
		char *ptr;
		int  c;
		
		setbuf(stdout,NULL);   /*set unbuffered*/
		for(ptr = str;(c = *ptr++) != 0;)
				putc(c,stdout);
}