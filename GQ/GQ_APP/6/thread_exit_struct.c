#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

struct menber
{
    int a;
    char *b;
}temp={8,"zieckey"};
void *create(void *arg)
{
    printf("new thread ... \n");
    return (void *)&temp;
}

int main(int argc,char *argv[])
{
    int error;
    pthread_t tid;
    struct menber *c;

    error = pthread_create(&tid, NULL, create, NULL);
    
    if( error )
    {
        printf("new thread is not created ... \n");
        return -1;
    }
    printf("main ... \n");

    error = pthread_join(tid,(void *)&c);

    if( error )
    {
        printf("new thread is not exit ... \n");
        return -2;
    }
    printf("c->a = %d  \n",c->a);
    printf("c->b = %s  \n",c->b);
    sleep(1);
    return 0;
}