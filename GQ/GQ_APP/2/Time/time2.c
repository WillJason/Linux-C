#include <time.h>
#include <stdio.h>

int main(void)
{
    struct tm *ptr;
    time_t lt;
    
    /*��ȡ����ʱ��*/
    lt=time(NULL);
    
    /*ת��Ϊ��������ʱ��*/
    ptr=gmtime(&lt);
    
    /*�Ը�������ʱ����ַ�����ʽ��ӡ*/
    printf(asctime(ptr));
    
    /*�Ա���ʱ����ַ�����ʽ��ӡ*/
    printf(ctime(&lt));
    return 0;
}
