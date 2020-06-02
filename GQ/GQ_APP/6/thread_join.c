#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

void *thread(void *str)
{
    int i;
    for (i = 0; i < 10; ++i)
    {
        sleep(2);
        printf( "This in the thread : %d\n" , i );
    }
    return NULL;
}

int main()
{
    pthread_t pth;
    int i;
    int ret = pthread_create(&pth, NULL, thread, (void *)(i));
    
    pthread_join(pth, NULL);
    printf("123\n");
    for (i = 0; i < 10; ++i)
    {
        //sleep(1);
        printf( "This in the main : %d\n" , i );
    }
    
    return 0;
}