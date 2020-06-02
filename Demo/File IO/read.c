//标准输入输出头文件
#include <stdio.h>

//文件操作函数头文件
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define MAX_SIZE 1000

main(){
	int fd;
	ssize_t length_w,length_r = MAX_SIZE,ret;
	char *testwrite = "/bin/testwrite";
	char buffer_write[] = "Hello Write Function!";
	char buffer_read[MAX_SIZE];
	
	if((fd = open(testwrite,O_RDWR|O_CREAT,0777))<0){
		printf("open %s failed!\n",testwrite);
	}
	length_w = write(fd,buffer_write,strlen(buffer_write));
	if(length_w == -1){
		perror("write");
	}
	else{
		printf("Write Function OK!\n");
	}
	close(fd);
	
	if((fd = open(testwrite,O_RDWR|O_CREAT,0777))<0){
		printf("open %s failed!\n",testwrite);
	}
	if(ret = read(fd,buffer_read,length_r)){
		perror("read");
	}
	printf("Files Content is %s \n",buffer_read);
	close(fd);
}