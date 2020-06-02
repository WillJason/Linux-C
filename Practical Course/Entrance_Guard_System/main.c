/*
��ʼ->RFID��ʼ��->Ѱ��->�ж��п��޿�->��������ʾ��->����->ָʾ������->
��������ʾ��->��¼����ʱ��->��¼�ſ���Ϣ->����
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
	
	PcdAntennaOn();//����RC522���߷���
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
��д��ϵͳ��RFID�������������Լ�����
extern char PcdReset()       //��λ����ʼ��RC500
extern char PcdRequest()     //Ѱ��
extern char PcdAnticoll()    //����ͻ
extern char PcdSelect()      //ѡ��һ�ſ�
extern char ChangeCodeKey()  //ת����Կ��ʽ
extern char PcdAuthKey()     //������Կ
extern char PcdAuthstate()   //��֤��Կ
extern char PcdRead()        //����
extern char PcdWrite()       //д��
extern char PcdValue()       //��ֵ
extern char PcdRestore()     //ȡֵ
extern char PcdTransfer()    //����ֵ
extern char PcdHalt()        //������
extern char PcdReadE2()      //��rc500-eeprom����
extern char PcdWriteE2()     //д���ݵ�rc500-eeprom
extern char PcdConfigRestore()//�ָ�rc500��������
extern char PcdLoadConfig()  //ȡrc500-eeprom���Զ��������
extern char PcdComTransceive() //��RC500ͨѶ
extern char WriteRC()        //дRC500�Ĵ���
extern char WriteRawRC()     //дRC500�Ĵ���
extern char ReadRC()         //��RC500�Ĵ���
extern char ReadRawRC()      //��RC500�Ĵ���
extern char SetBitMark()     //��RC500�Ĵ���λ
extern char ClearBitMark()   //��RC500�Ĵ���λ
extern char PcdSetTmo()      //����RC500��ʱ��
extern char DelayMs()        //��ʱ
extern char DelayUs()        //��ʱ

*/