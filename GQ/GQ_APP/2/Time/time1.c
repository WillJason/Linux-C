#include <time.h>
#include <stdio.h>

int main(void){
    struct tm *local;
    time_t t;

    /* ��ȡ����ʱ�� */
    t=time(NULL);
    
    /* ������ʱ��ת��Ϊ����ʱ�� */
    local=localtime(&t);
    /*��ӡ��ǰ��Сʱֵ*/
    printf("Local hour is: %d\n",local->tm_hour);
    
    /* ������ʱ��ת��Ϊ��������ʱ�� */
    local=gmtime(&t);
    printf("UTC hour is: %d\n",local->tm_hour);
    return 0;
}
