#include <stdio.h>
#include <pthread.h>
#include <unistd.h> /*getpid()*/

void *create(void *arg)
{
    printf("New thread .... \n");
    printf("This thread's id is %u  \n", (unsigned int)pthread_self());
    printf("The process pid is %d  \n",getpid());
    return (void *)0;
} 

int main(int argc,char *argv[])
{
    pthread_t tid;
    int error;

    printf("Main thread is starting ... \n");

    error = pthread_create(&tid, NULL, create, NULL);

    if(error)
    {
        printf("thread is not created ... \n");
        return -1;
    }
    printf("The main process's pid is %d  \n",getpid());
    sleep(1);
    return 0;
}
