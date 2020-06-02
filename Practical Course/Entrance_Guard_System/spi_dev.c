/*
 * SPI testing utility (using spidev driver)
 *
 * Copyright (c) 2007  MontaVista Software, Inc.
 * Copyright (c) 2007  Anton Vorontsov <avorontsov@ru.mvista.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License.
 *
 * Cross-compile with cross-gcc -I/path/to/cross-kernel/include
 */

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>

#include "spi_dev.h"
#include "rc522.h"

int spi_rc522_fd = 0;
unsigned char UID[5], Temp[4];

int WriteRC(int addr, int data)
{
	int ret;
	int fd = spi_rc522_fd;
	unsigned char  TxBuf[2];

	//bit7:MSB=0,bit6~1:addr,bit0:RFU=0
	TxBuf[0] = ((unsigned char)addr << 1)&0x7E;

	TxBuf[1] = (unsigned char)data;
	
	ret = write(fd, TxBuf, 2);
	if (ret < 0)
		printf("spi:SPI Write error\n");

	usleep(10);

	return ret;
}

unsigned char ReadRC(int addr)
{
	int ret;
	int fd = spi_rc522_fd;
	unsigned char  ReData;
	unsigned char  Address;
	
	Address  = (unsigned char)addr << 1;
	Address |= (1 << 7);
	Address &= ~(1 << 0);
	
	ret = write(fd, &Address, 1);
	if (ret < 0)
		printf("spi:SPI Write error\n");

	usleep(100);

	ret = read(fd, &ReData, 1);
	if (ret < 0)
		printf("spi:SPI Read error\n");

	return ReData;
}

void SetBitMark(unsigned char reg,unsigned char mask)  
{
  	char tmp = 0x0;
	
  	tmp = ReadRC(reg) | mask;
	
  	WriteRC(reg,tmp | mask);
}

//******************************************************************/
//��    �ܣ���RC522�Ĵ���λ
//����˵����reg[IN]:�Ĵ�����ַ
//          mask[IN]:��λֵ
//******************************************************************/
void ClearBitMark(unsigned char reg, unsigned char mask)  
{
	char tmp = 0x0;
	
	tmp = ReadRC(reg)&(~mask);
	
	WriteRC(reg, tmp);  // clear bit mark
}

int rc522_init()
{
	char version = 0;
	//reset
	WriteRC(CommandReg, PCD_RESETPHASE);
	usleep(10);
	WriteRC(ModeReg, 0x3D);
	WriteRC(TReloadRegL, 30);
	WriteRC(TReloadRegH, 0);
	WriteRC(TModeReg, 0x8D);
	WriteRC(TPrescalerReg, 0x3E);

	version = ReadRC(VersionReg);
	printf("Chip Version: 0x%x\n", version);
	usleep(50000);

	return version;
}
/*����rc522���߷���*/
void PcdAntennaOn()
{
	unsigned char i;
  
	WriteRC(TxASKReg, 0x40);
  	usleep(20);
  
	i = ReadRC(TxControlReg);
  if(!(i&0x03))
    SetBitMark(TxControlReg, 0x03);
  	
	i = ReadRC(TxASKReg);
}

/******************************************************************
function  ��ͨ��RC522��ISO14443��ͨѶ
args      ��Command[IN]:RC522������
            pInData[IN]:ͨ��RC522���͵���Ƭ������
            InLenByte[IN]:�������ݵ��ֽڳ���
            pOutData[OUT]:���յ��Ŀ�Ƭ��������
            *pOutLenBit[OUT]:�������ݵ�λ����
******************************************************************/
char PcdComRC522(unsigned char Command, unsigned char *pInData, 
                 				unsigned char InLenByte, unsigned char *pOutData, 
                 				unsigned int  *pOutLenBit)
{
	char status = MI_ERR;
	unsigned char irqEn  = 0x00;
	unsigned char waitFor = 0x00;
	unsigned char lastBits;
	unsigned char n;
	unsigned int  i;
	
	switch (Command)
	{
		case PCD_AUTHENT:
	  		irqEn   = 0x12;
			  waitFor = 0x10;
			  break;
		case PCD_TRANSCEIVE:
	  		irqEn   = 0x77;
	  		waitFor = 0x30;
	  		break;
		default:
	  		break;
	}
	
	WriteRC(ComIEnReg, irqEn|0x80);
	ClearBitMark(ComIrqReg, 0x80);
	WriteRC(CommandReg, PCD_IDLE);
	SetBitMark(FIFOLevelReg, 0x80); // ���FIFO 
	for(i=0; i<InLenByte; i++)
		WriteRC(FIFODataReg, pInData[i]); // ����д��FIFO 
		
	WriteRC(CommandReg, Command); // ����д������Ĵ���

	if(Command == PCD_TRANSCEIVE)
		SetBitMark(BitFramingReg,0x80); // ��ʼ����     

	i = 6000; //����ʱ��Ƶ�ʵ���������M1�����ȴ�ʱ��25ms
  	do 
	{
		n = ReadRC(ComIrqReg);
		i--;
	}
	while((i!=0)&&!(n&0x01)&&!(n&waitFor));
	
	ClearBitMark(BitFramingReg, 0x80);

	if(i!=0)
	{
		if(!(ReadRC(ErrorReg) & 0x1B))
		{
			status = MI_OK;
			if (n&irqEn&0x01)
				status = MI_NOTAGERR;
			if(Command == PCD_TRANSCEIVE)
			{
				n = ReadRC(FIFOLevelReg);
				
				lastBits = ReadRC(ControlReg) & 0x07;
				if(lastBits)
					*pOutLenBit = (n-1)*8 + lastBits;
				else
					*pOutLenBit = n*8;

				if(n == 0)
					n = 1;
				if(n>MAXRLEN)
					n = MAXRLEN;

				for (i=0; i<n; i++)
					pOutData[i] = ReadRC(FIFODataReg); 
			}
		}
		else
		{
			status = MI_ERR;
		}
	}

	SetBitMark(ControlReg, 0x80);// stop timer now
	WriteRC(CommandReg, PCD_IDLE); 

	return status;
}

char PcdSendRequest(unsigned char req_code, unsigned char *pTagType)
{
	char status;  
	unsigned int  unLen;
	unsigned char ucComMF522Buf[MAXRLEN]; 

	ClearBitMark(Status2Reg, 0x08);
	WriteRC(BitFramingReg, 0x07);
	SetBitMark(TxControlReg, 0x03);
 
  	ucComMF522Buf[0] = req_code;

  	status = PcdComRC522(PCD_TRANSCEIVE, ucComMF522Buf,
                       				1, ucComMF522Buf, &unLen);

  	if ((status == MI_OK) && (unLen == 0x10))
  	{    
    		*pTagType     = ucComMF522Buf[0];
    		*(pTagType+1) = ucComMF522Buf[1];
  	}
  	else
	{
		status = MI_ERR;
  	}

  	return status;
}

//******************************************************************/
//��    �ܣ�����ͻ                                                  /
//����˵��: pSnr[OUT]:��Ƭ���кţ�4�ֽ�                             /
//��    ��: �ɹ�����MI_OK                                           /
//******************************************************************/
char PcdAnticoll(unsigned char *pSnr)
{
	char status;
	unsigned char i, snr_check = 0;
	unsigned int  unLen;
	unsigned char ucComMF522Buf[MAXRLEN]; 
    
	ClearBitMark(Status2Reg, 0x08);
	WriteRC(BitFramingReg, 0x00);
	ClearBitMark(CollReg, 0x80);
 
	ucComMF522Buf[0] = PICC_ANTICOLL1;
	ucComMF522Buf[1] = 0x20;

    	status = PcdComRC522(PCD_TRANSCEIVE, ucComMF522Buf,
								2, ucComMF522Buf, &unLen);

    	if(status == MI_OK)
	{
		for (i=0; i<4; i++)
		{   
			*(pSnr+i)  = ucComMF522Buf[i];
			snr_check ^= ucComMF522Buf[i];
		}
		if (snr_check != ucComMF522Buf[i])
		{
			status = MI_ERR;
		}
	}
    
	SetBitMark(CollReg,0x80);
	
	return status;
}



void Auto_Seek(void)
{
	int i = 0;
	unsigned long num = 0;
	
		if(PcdSendRequest(0x52,Temp) == MI_OK)
		{
			if(Temp[0]==0x04 && Temp[1]==0x00)  
				printf("MFOne-S50\n");
			else if(Temp[0]==0x02 && Temp[1]==0x00)
				printf("MFOne-S70\n");
			else if(Temp[0]==0x44 && Temp[1]==0x00)
				printf("MF-UltraLight\n");
			else if(Temp[0]==0x08 && Temp[1]==0x00)
				printf("MF-Pro\n");
			else if(Temp[0]==0x44 && Temp[1]==0x03)
				printf("MF Desire\n");
			else
				printf("Unknown\n");
			
			if(PcdAnticoll(UID) == MI_OK)
			{ 
				printf("Card Id is(%d):", num++);
				
				for(i=0; i<4; i++)
					printf("%x", UID[i]);
				printf("\n");
				
				PcdSendRequest(0x52,Temp);//clear
			}
			else
			{
				printf("no serial num read\n");
			}
		}
		else
		{
			printf("No Card!\n");
		}

		usleep(300000);
//	} 
}

void Config_Handler(unsigned char inputvalue)
{
  switch(toupper(inputvalue)) 
  {
    case 'A':
             Auto_Seek();
              break;
    case 'L':
    case 'K':
   	case 'F':
    case 'M':
              break;
    default:
             Auto_Seek();
  }
}
