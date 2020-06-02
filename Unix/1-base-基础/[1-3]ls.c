/*
ls命令的简要实现
（1）apue.h是作者自定义的一个头文件，并不是Unix/lLinux系统自带的，所以需要到www.apuebook.com下载资源代码。err_qiut和err_sys也包含在apue的压缩文件里的error.c中，是作者自定义的错误处理函数。

（2）下载完之后将src.3e.tar.gz解压。tar -zvxf src.3e.tar.gz

（3）进入解压文件 。cd apue.3e

（4）make ?如果没有输入这个命令，在lib文件中无法看到libapue.a

（5）将apue.h头文件、libapue.a静态库和error.c文件复制到编译默认的路径下。必须在root用户下才有权限

$ cp ./include/apue.h /usr/include

$ cp ./lib/libapue.a /usr/lib

$ cp ./lib/error.c /usr/lib

（6）make

（7）在ls(1)程序中包含头文件#include "error.c"

（8）gcc 编译运行
*/
#include <apue.h>
#include <dirent.h>

int main (int argc,char *argv[])
{
		DIR  *dp;
		struct dirent  *dirp;
		
		if(argc!=2)
			err_quit("usage:ls directory_name");
			
		if(dp=opendir(argv[1])==NULL)
			err_sys("can't open %s",argv[1]);
			
		while((dirp=readddir(dp))!=NULL)
			printf("%s\n",dirp->d.name);
			
		closedir(dp);
		exit(0);
}




