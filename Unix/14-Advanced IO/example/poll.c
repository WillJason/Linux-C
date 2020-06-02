/*
采用poll实现对标准输入和文件的循环监听。
*/
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
 
#include <poll.h>
#include <fcntl.h>
#include <unistd.h>
 
#define MAX_BUFFER_SIZE 1024
#define IN_FILES 3
#define TIME_DELAY 60*5
#define MAX(a,b) ((a>b)?(a):(b))
 
int main(int argc ,char **argv)
{
  struct pollfd fds[IN_FILES];
  char buf[MAX_BUFFER_SIZE];
  int i,res,real_read, maxfd;
  fds[0].fd = 0;																			 //设置对标准输入监听
  if((fds[1].fd=open("data1",O_RDONLY|O_NONBLOCK)) < 0)//对data1文件监听
    {
      printf("open data1 error:%s",strerror(errno));
      return 1;
    }
  if((fds[2].fd=open("data2",O_RDONLY|O_NONBLOCK)) < 0)//对data2文件监听
    {
      printf("open data2 error:%s",strerror(errno));
      return 1;
    }
  for (i = 0; i < IN_FILES; i++)//均设置为不阻塞读取
    {
      fds[i].events = POLLIN;
    }
    for(;;)//循环读取，直到出现返回
    {
    	////设置监视
      if (poll(fds, IN_FILES, TIME_DELAY) <= 0)
    	{
     	   printf("Poll error\n");
     	  return 1;
    	}
      	for (i = 0; i< IN_FILES; i++)//当对每个监听轮询
    	{
     	   if (fds[i].revents == POLLIN)//当有数据可读的时候
       	  {
		         	memset(buf, 0, MAX_BUFFER_SIZE);
		         	real_read = read(fds[i].fd, buf, MAX_BUFFER_SIZE);//读取监听的数据
		         	if (real_read < 0)
		        	{
		         	   if (errno != EAGAIN)
		           	   {
		             	      return 1;
		           	   }
		        	}
		         	else if (!real_read)//未读取到数据
		        	{
		         	   close(fds[i].fd);
		         	   fds[i].events = 0;
		        	}
         			else
		        	{
		         	   if (i == 0)//如果监听的是标准输入
		          	   {
		             			if ((buf[0] == 'q') || (buf[0] == 'Q'))
		            			{
			             			return 1;
			           		  }
					         		else
					         		{
												buf[real_read] = '\0';
			                        printf("%s", buf);
											}
		           	   }
		         	   else//如果监听的是data文件则执行该部分
		           	   {
		             			buf[real_read] = '\0';
		             			printf("%s", buf);
		           	   }
		        		}
       	  }
    	}
    }
  exit(0);
}
