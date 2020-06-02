/*
我们只需GPRMC（六组),每组数据之间有换行符
速率 海里/小时
*/
#include <stdio.h>      /*标准输入输出定义*/  
#include <stdlib.h>     /*标准函数库定义*/  
#include <unistd.h>     /*Unix 标准函数定义*/  
#include <sys/types.h>  
#include <sys/stat.h>  
#include <errno.h>  
#include <string.h>  
#include <fcntl.h>  
#include <termios.h>  
  
#define  Buff_Size 512  
#define  MAX_COM_NUM 3  
  
int  Section_ID=0,i=0;  
  
 struct data  
{  
    char GPS_time[20];          //UTC时间  
    char GPS_satellite;               //使用卫星  
    char GPS_lat[12];           //纬度  
    char GPS_lon[12];           //经度  
    //char GPS_warn;             //定位警告   
    char GPS_speed[5];         //速度  
    char GPS_date[8];          //UTC日期            
          
}GPS_DATA;  
  
  
  
int port_config(int fd,int baud_rate,int data_bits,char parity,int stop_bits)  
{  
    struct termios new_cfg,old_cfg;  
    int speed;  
    //保存并测试现有串口参数设置，在这里如果串口号出错，会有相关的出错信息  
      
    if(tcgetattr(fd,&old_cfg)!=0)  
    {  
        perror("tcgetattr");  
        return -1;  
    }  
     tcflush(fd, TCIOFLUSH);  //清除所有正在发生的I/O数据
    new_cfg=old_cfg;  				
    cfmakeraw(&new_cfg);			//配置为原始模式  
    new_cfg.c_cflag&=~CSIZE;  //先屏蔽其他标志位
  
    //设置波特率  
    switch(baud_rate)  
    {  
        case 2400:  
        {  
            speed = B2400;  
            break;  
        }  
        case 4800:  
        {  
            speed = B4800;  
            break;  
        }  
        case 9600:  
        {  
            speed = B9600;  
            break;  
        }  
        case 19200:  
        {  
            speed = B19200;  
            break;  
        }  
        case 38400:  
        {  
            speed = B38400;  
            break;  
        }  
        case 115200:  
        {  
            speed = B115200;  
            break;   
        }  
          
  
    }  
    cfsetispeed(&new_cfg,speed); //设置波特率 
    cfsetospeed(&new_cfg,speed);  
    //设置数据位  
  
    switch(data_bits)  
    {  
        case 7:  
        {  
            new_cfg.c_cflag|=CS7;  //将数据位修改为7bit
            break;  
        }  
          
        case 8:  
        {  
            new_cfg.c_cflag|=CS8;  //将数据位修改为8bit
            break;  
        }  
          
    }  
  
    //设置停止位  
    switch(stop_bits)  
    {  
        case 1:  
        {  
            new_cfg.c_cflag &=~CSTOPB;  
            break;  
        }  
  
        case 2:  
        {  
            new_cfg.c_cflag |=CSTOPB;  
            break;  
        }  
          
          
    }  
  
    //设置奇偶校验位  
    switch(parity)  
    {  
        case 'o':  
        case 'O':  
        {  
            new_cfg.c_cflag|=(PARODD|PARENB);  
            new_cfg.c_iflag|=(INPCK |ISTRIP);  
            break;  
        }  
        case 'e':  
        case 'E':  
        {  
            new_cfg.c_cflag |=PARENB;  
            new_cfg.c_cflag &=~PARODD;  
            new_cfg.c_iflag |=(INPCK | ISTRIP);  
            break;  
        }  
        case 's':  
        case 'S':  
        {  
            new_cfg.c_cflag &=~PARENB;  
            new_cfg.c_cflag &=~CSTOPB;  
            break;  
        }  
          
        case 'n':  
        case 'N':  
        {  
            new_cfg.c_cflag &=~PARENB;  
            new_cfg.c_iflag &=~INPCK;  
            break;  
        }  
          
    }  
	  /*
	  控制符VTIME和VMIN之间有着复杂的关系。
	
		1、VTIME定义要求等待的零到几百毫秒的时间量(通常是一个8位的unsigned char变量，取值不能大于cc_t)。
		2、VMIN定义了要求等待的最小字节数(不是要求读的字节数——read()的第三个参数才是指定要求读的最大字
		节数)，这个字节数可能是0。
	
		如果VTIME取0，VMIN定义了要求等待读取的最小字节数。函数read()只有在读取了VMIN个字节的数据或者收到
		一个信号的时候才返回。
		如果VMIN取0，VTIME定义了即使没有数据可以读取，read()函数返回前也要等待几百毫秒的时间量。这时，
		read()函数不需要像其通常情况那样要遇到一个文件结束标志才返回0。
		如果VTIME和VMIN都不取0，VTIME定义的是当接收到第一个字节的数据后开始计算等待的时间量。如果当调用
		read函数时可以得到数据，计时器马上开始计时。如果当调用read函数
		时还没有任何数据可读，则等接收到第一个字节的数据后，计时器开始计时。函数read可能会在读取到VMIN个
		字节的数据后返回，也可能在计时完毕后返回，这主要取决于哪个条件首先实现。不过函数至少会读取到一个
		字节的数据，因为计时器是在读取到第一个数据时开始计时的。
		如果VTIME和VMIN都取0，即使读取不到任何数据，函数read也会立即返回。同时，返回值0表示read函数不需
		要等待文件结束标志就返回了。
	  */
    new_cfg.c_cc[VTIME] =10;  
    new_cfg.c_cc[VMIN] =5;  
    //处理未接收字符  
    tcflush(fd,TCIFLUSH); //清除正收到的数据，且不会读取出来。
       
    if((tcsetattr(fd,TCSANOW,&new_cfg))!=0)  
    {  
        perror("tcsetattr");  
        return -1;  
    }  
      
    return 0;  
}  
  
  
//打开串口函数  
int open_dev(char* port)  
{  
    int fd;  
    char* dev = port;  
        //打开串口  
        if((fd=open(dev,O_RDWR|O_NOCTTY|O_NDELAY))<0)  
        {  
            perror("open serial port");  
            return -1;  
        }  
        //恢复串口为堵塞状态  
        if(fcntl(fd,F_SETFL,0) <0 )  
        {  
            perror("fcntl F_SETFL\n");  
            return -1;  
              
        }  
        //测试是否为终端设备  
        if(isatty(STDIN_FILENO)==0)  
        {  
            perror("standard input is not a terminal device");  
        }  
        return fd;  
}  
  
void print_data(void)  
{  
    //打印选择界面，即引导的字符号  
    printf("Now the receive time is %s\n",GPS_DATA.GPS_time);  
    printf("The star is %c 3\n",GPS_DATA.GPS_satellite);  
    printf("The earth latitude is :%s\n",GPS_DATA.GPS_lat);  
    printf("The earth longitude is :%s\n",GPS_DATA.GPS_lon);   
    printf("The train speed is:%s km/h\n",GPS_DATA.GPS_speed);  
    printf("The date is:%s\n",GPS_DATA.GPS_date);  
      
}  
  
  
void GPS_Hold_GPRMC(char data)  
{  
		//$GPRMC,101751.00,A,4148.2214,N,12324.1943,E,0.895,82.800,051019,,E,A*27    
    if(data==',')  
    {  
        ++Section_ID;  
        i=0;  
    }  
    else  
    {  
        switch(Section_ID)  
        {  
            case 1://02:48:13         
                    GPS_DATA.GPS_time[i++]=data;          
                    if(i==2 || i==5)  
                    {         
                        GPS_DATA.GPS_time[i++]=':';       
                    }                 
                    GPS_DATA.GPS_time[8]='\0';  
                break;  
            case 2:  
                if(data=='A')  
                    GPS_DATA.GPS_satellite='>';  
                else  
                    GPS_DATA.GPS_satellite='<';  
                break;  
            case 3://3158.4608  
                    GPS_DATA.GPS_lat[++i]=data;    
                    GPS_DATA.GPS_lat[12]='\0';                     
                break;  
                  
            case 4:  
                if(data=='N')  
                    GPS_DATA.GPS_lat[0]='N';  
                else if(data=='S')  
                    GPS_DATA.GPS_lat[0]='S';  
              
                break;  
            case 5://11848.3737,E  
                  
                    GPS_DATA.GPS_lon[++i]=data;    
                    GPS_DATA.GPS_lon[12]='\0';  
                break;  
                  
            case 6:  
                if(data=='E')  
                    GPS_DATA.GPS_lon[0]='E';  
                else if(data=='W')  
                    GPS_DATA.GPS_lon[0]='W';  
                  
                break;  
            case 7://10.05  
                    GPS_DATA.GPS_speed[i++]=data;  
                    GPS_DATA.GPS_speed[4]='\0';                       
                break;  
            case 9://15-07-06 -> 06-07-15  
                    GPS_DATA.GPS_date[i++]=data;      
                    if(i==2 || i==5)                          
                    {  
                        GPS_DATA.GPS_date[i++]='-';  
                    }                                 
                    GPS_DATA.GPS_date[8]='\0';                    
                break;  
        }  
    }         
  
}  
  
void read_data(int fd)  
{  
    char buffer[Buff_Size],dest[1024];     
    char array[10]="$GPRMC";  
    int  res,i=0,j=0,k;  
    int data=1,len=0;  
    memset(dest,0,sizeof(dest));  
      
		do  
		{      
     memset(buffer,0,sizeof(buffer));  
		//$GPRMC,101751.00,A,4148.2214,N,12324.1943,E,0.895,82.800,051019,,E,A*27  
        if(res=read(fd,buffer,1)>0)  
        {         
            //此处源源不断传入参数，一次读到数据可能为（$GPRMC,024），res为读到长度，现在把每一位传入函数处理；     
            strcat(dest,buffer);  
            if(buffer[0]=='\n')  
            {  
                i=0;  
                if(strncmp(dest,array,6)==0)  
                {                 
                        printf("%s",dest);  
                        len=strlen(dest);  
                        for(k=0;k<len;k++)  
                        {  
                            GPS_Hold_GPRMC(dest[k]);  
                        }         
                        Section_ID=0;  
                              
                        print_data();  
                }  
                bzero(dest,sizeof(dest));  
            }  
                  
        }  
		}while(1);  
    close(fd);  
}  
  
int main(int argc,char*argv[])  
{         
    int fd=0;        
    char* GPS_dev = argv[1];      
		if(2 != argc)
		{
      printf("Usage:  gps_test [uart port]\r\n");              
      return -1;
    }

    fd=open_dev(GPS_dev);  
    if(fd<0)   
    {  
       perror("open fail!");  
    }  
    printf("open sucess!\n");  
    if((port_config(fd,9600,8,'N',1))<0)    
    {  
       perror("port_config fail!\n");  
    }  
    printf("The received worlds are:\n");  
    read_data(fd);  
    return 0;  
}  
