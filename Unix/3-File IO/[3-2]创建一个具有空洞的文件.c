/*
文件偏移量可以大于文件的当前长度，在这种情况下，对该文件的下一次写将加长该文件，
并在文件中构成一个空洞。位于文件中但没有写过的字节都被读为0.
*/
#include <apue.h>
#include <fcntl.h>

char buf1[] = "abcdefghij"
char buf2[] = "ABCDEFGHIJ"

int main(void)
{
	int fd;
	
	if((fd = creat("file.hole",FILE_MODE))<0)
		err_sys("creat error");
	
	if(write(fd,buf1,10) != 10)
		err_sys("buf1 write error");
		
	if(lseek(fd,16384,SEEK_SET) == -1)
		err_sys("lseek error");
	/* offset now = 16384 */
	
	if(write(fd,buf2,10) != 10)
		err_sys("buf2 write error");
	/* offset now = 16394 */

	exit(0);
}
/*使用od（1）命令观察该文件的实际内容。命令行中的-c标志表示以字符方式打印文件内容*/