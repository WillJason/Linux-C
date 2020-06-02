#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void *create(void *arg)
{
    char *name;
    name=(char *)arg;
    printf("The parameter passed from main function is %s  \n",name);
    return (void *)0;
}

int main(int argc, char *argv[])
{
    char *a="zieckey";
    int error;
    pthread_t tidp;

    error=pthread_create(&tidp, NULL, create, (void *)a);

    if(error!=0)
    {
        printf("pthread is not created.\n");
        return -1;
    }
    sleep(1);
    printf("pthread is created... \n");
    return 0;
}    

