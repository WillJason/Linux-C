#include <time.h>
#include <stdio.h>

int main(void)
{
    struct tm *ptr;
    time_t lt;
    
    /*获取日历时间*/
    lt=time(NULL);
    
    /*转化为格林威治时间*/
    ptr=gmtime(&lt);
    
    /*以格林威治时间的字符串方式打印*/
    printf(asctime(ptr));
    
    /*以本地时间的字符串方式打印*/
    printf(ctime(&lt));
    return 0;
}
