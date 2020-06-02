/*
开始->RFID初始化->寻卡->判断有卡无卡->蜂鸣器提示音->开门->指示灯亮起->
蜂鸣器提示音->记录开门时间->记录门卡信息->关门
*/
#include "main.h"

extern int rc522_init(void);
extern void PcdAntennaOn();
extern void Config_Handler(unsigned char inputvalue);

extern void led_set(char *led_dev,int lednum,int cmd);
extern void buzzer_set(char *buzzer_dev , int state);
extern void relay_set(char *relay_dev,int state);

extern void rtc_record(char *rtc_dev,char *FILE);
extern void data_record(char *FILE);

extern unsigned char UID[5];
extern int spi_rc522_fd;
//No card reader led all ON
//Find card state LED1 ON,LED2 OFF
//Open the door led all OFF

int main(void)
{
	spi_rc522_fd = open(Device_RC522, O_RDWR);
	if (spi_rc522_fd < 0){
		perror(Device_RC522);
		return -1;
	}
		
	if(rc522_init()==0xff){
		printf("No Card Reader\n");
		
		led_set(Device_LEDS,Led_On,led_0);
		led_set(Device_LEDS,Led_On,led_1);
		
		return -1;
	}else{
		led_set(Device_LEDS,Led_On,led_0);
		led_set(Device_LEDS,Led_Off,led_1);
	}
	
	PcdAntennaOn();//开启RC522天线发射
	Config_Handler('A');
	close(spi_rc522_fd);
	//if find card ,record date and cardnumber
	if(UID[0]!='\0'){
		buzzer_set(Device_BUZZER,Buzzer_On);
		
		relay_set(Device_RELAY,Relay_On);
		
		led_set(Device_LEDS,Led_Off,led_0);
		led_set(Device_LEDS,Led_Off,led_1);
		
		rtc_record(Device_RTC,FILE);
		data_record(FILE);
	}
	else{
		printf("No Card\n");
		return -1;
	}
	buzzer_set(Device_BUZZER,Buzzer_Off);
	
	sleep(1);
	relay_set(Device_RELAY,Relay_Off);
	
	led_set(Device_LEDS,Led_On,led_0);
	led_set(Device_LEDS,Led_Off,led_1);
	
	return 0;
}

/*
读写器系统的RFID函数常用名字以及命令
extern char PcdReset()       //复位并初始化RC500
extern char PcdRequest()     //寻卡
extern char PcdAnticoll()    //防冲突
extern char PcdSelect()      //选定一张卡
extern char ChangeCodeKey()  //转换秘钥格式
extern char PcdAuthKey()     //传送密钥
extern char PcdAuthstate()   //验证密钥
extern char PcdRead()        //读块
extern char PcdWrite()       //写块
extern char PcdValue()       //加值
extern char PcdRestore()     //取值
extern char PcdTransfer()    //备份值
extern char PcdHalt()        //卡休眠
extern char PcdReadE2()      //读rc500-eeprom数据
extern char PcdWriteE2()     //写数据到rc500-eeprom
extern char PcdConfigRestore()//恢复rc500出厂设置
extern char PcdLoadConfig()  //取rc500-eeprom中自定义的设置
extern char PcdComTransceive() //和RC500通讯
extern char WriteRC()        //写RC500寄存器
extern char WriteRawRC()     //写RC500寄存器
extern char ReadRC()         //读RC500寄存器
extern char ReadRawRC()      //读RC500寄存器
extern char SetBitMark()     //置RC500寄存器位
extern char ClearBitMark()   //清RC500寄存器位
extern char PcdSetTmo()      //设置RC500定时器
extern char DelayMs()        //延时
extern char DelayUs()        //延时

*/