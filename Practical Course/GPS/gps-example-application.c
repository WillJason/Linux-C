/*
����ֻ��GPRMC������),ÿ������֮���л��з�
���� ����/Сʱ
*/
#include <stdio.h>      /*��׼�����������*/  
#include <stdlib.h>     /*��׼�����ⶨ��*/  
#include <unistd.h>     /*Unix ��׼��������*/  
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
    char GPS_time[20];          //UTCʱ��  
    char GPS_satellite;               //ʹ������  
    char GPS_lat[12];           //γ��  
    char GPS_lon[12];           //����  
    //char GPS_warn;             //��λ����   
    char GPS_speed[5];         //�ٶ�  
    char GPS_date[8];          //UTC����            
          
}GPS_DATA;  
  
  
  
int port_config(int fd,int baud_rate,int data_bits,char parity,int stop_bits)  
{  
    struct termios new_cfg,old_cfg;  
    int speed;  
    //���沢�������д��ڲ������ã�������������ںų���������صĳ�����Ϣ  
      
    if(tcgetattr(fd,&old_cfg)!=0)  
    {  
        perror("tcgetattr");  
        return -1;  
    }  
     tcflush(fd, TCIOFLUSH);  //����������ڷ�����I/O����
    new_cfg=old_cfg;  				
    cfmakeraw(&new_cfg);			//����Ϊԭʼģʽ  
    new_cfg.c_cflag&=~CSIZE;  //������������־λ
  
    //���ò�����  
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
    cfsetispeed(&new_cfg,speed); //���ò����� 
    cfsetospeed(&new_cfg,speed);  
    //��������λ  
  
    switch(data_bits)  
    {  
        case 7:  
        {  
            new_cfg.c_cflag|=CS7;  //������λ�޸�Ϊ7bit
            break;  
        }  
          
        case 8:  
        {  
            new_cfg.c_cflag|=CS8;  //������λ�޸�Ϊ8bit
            break;  
        }  
          
    }  
  
    //����ֹͣλ  
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
  
    //������żУ��λ  
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
	  ���Ʒ�VTIME��VMIN֮�����Ÿ��ӵĹ�ϵ��
	
		1��VTIME����Ҫ��ȴ����㵽���ٺ����ʱ����(ͨ����һ��8λ��unsigned char������ȡֵ���ܴ���cc_t)��
		2��VMIN������Ҫ��ȴ�����С�ֽ���(����Ҫ������ֽ�������read()�ĵ�������������ָ��Ҫ����������
		����)������ֽ���������0��
	
		���VTIMEȡ0��VMIN������Ҫ��ȴ���ȡ����С�ֽ���������read()ֻ���ڶ�ȡ��VMIN���ֽڵ����ݻ����յ�
		һ���źŵ�ʱ��ŷ��ء�
		���VMINȡ0��VTIME�����˼�ʹû�����ݿ��Զ�ȡ��read()��������ǰҲҪ�ȴ����ٺ����ʱ��������ʱ��
		read()��������Ҫ����ͨ���������Ҫ����һ���ļ�������־�ŷ���0��
		���VTIME��VMIN����ȡ0��VTIME������ǵ����յ���һ���ֽڵ����ݺ�ʼ����ȴ���ʱ���������������
		read����ʱ���Եõ����ݣ���ʱ�����Ͽ�ʼ��ʱ�����������read����
		ʱ��û���κ����ݿɶ�����Ƚ��յ���һ���ֽڵ����ݺ󣬼�ʱ����ʼ��ʱ������read���ܻ��ڶ�ȡ��VMIN��
		�ֽڵ����ݺ󷵻أ�Ҳ�����ڼ�ʱ��Ϻ󷵻أ�����Ҫȡ�����ĸ���������ʵ�֡������������ٻ��ȡ��һ��
		�ֽڵ����ݣ���Ϊ��ʱ�����ڶ�ȡ����һ������ʱ��ʼ��ʱ�ġ�
		���VTIME��VMIN��ȡ0����ʹ��ȡ�����κ����ݣ�����readҲ���������ء�ͬʱ������ֵ0��ʾread��������
		Ҫ�ȴ��ļ�������־�ͷ����ˡ�
	  */
    new_cfg.c_cc[VTIME] =10;  
    new_cfg.c_cc[VMIN] =5;  
    //����δ�����ַ�  
    tcflush(fd,TCIFLUSH); //������յ������ݣ��Ҳ����ȡ������
       
    if((tcsetattr(fd,TCSANOW,&new_cfg))!=0)  
    {  
        perror("tcsetattr");  
        return -1;  
    }  
      
    return 0;  
}  
  
  
//�򿪴��ں���  
int open_dev(char* port)  
{  
    int fd;  
    char* dev = port;  
        //�򿪴���  
        if((fd=open(dev,O_RDWR|O_NOCTTY|O_NDELAY))<0)  
        {  
            perror("open serial port");  
            return -1;  
        }  
        //�ָ�����Ϊ����״̬  
        if(fcntl(fd,F_SETFL,0) <0 )  
        {  
            perror("fcntl F_SETFL\n");  
            return -1;  
              
        }  
        //�����Ƿ�Ϊ�ն��豸  
        if(isatty(STDIN_FILENO)==0)  
        {  
            perror("standard input is not a terminal device");  
        }  
        return fd;  
}  
  
void print_data(void)  
{  
    //��ӡѡ����棬���������ַ���  
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
            //�˴�ԴԴ���ϴ��������һ�ζ������ݿ���Ϊ��$GPRMC,024����resΪ�������ȣ����ڰ�ÿһλ���뺯������     
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
