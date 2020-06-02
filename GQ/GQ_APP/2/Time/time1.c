#include <time.h>
#include <stdio.h>

int main(void){
    struct tm *local;
    time_t t;

    /* 获取日历时间 */
    t=time(NULL);
    
    /* 将日历时间转化为本地时间 */
    local=localtime(&t);
    /*打印当前的小时值*/
    printf("Local hour is: %d\n",local->tm_hour);
    
    /* 将日历时间转化为格林威治时间 */
    local=gmtime(&t);
    printf("UTC hour is: %d\n",local->tm_hour);
    return 0;
}
