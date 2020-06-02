/*
 *  Author: SDAM
 */

#include <stdint.h>

#include "section.h"
#include "ceva/csl/mss.h"
#include "dma/dma.h"
#include "dma/dma_legacy.h"

#include "error.h"
#include "typedef.h"
#include "loader/loader.h"
#include "status/status.h"
#include "work/work.h"
#include "utils/trace.h"
#include "uart/uart.h"
#include "mailbox/mailbox.h"

#define DMA_1D_TRANSFER  0
#define DMA_2D_TRANSFER  1

/* ͼƬ���������� */
struct zoom_params {
	uint32_t src_phy;
	uint32_t dst_phy;

	uint32_t Origin_width;
	uint32_t Origin_height;

	uint32_t Zoom_width;
	uint32_t Zoom_height;
	uint32_t iBpp;

	uint32_t mode;
	uint32_t cost_cycles;
};

/**********************************************************************
 * �������ƣ� PicZoom
 * ���������� ����ȡ����ֵ��������ͼƬ
 *            ע��ú���������ڴ���������ź��ͼƬ,�����Ҫ��free�����ͷŵ�
 *            "����ȡ����ֵ"��ԭ����ο�������"ͼ�������㷨"
 * ��������� 	ptOriginPic - �ں�ԭʼͼƬ����������
 *          ptZoomPic    - �ں����ź��ͼƬ����������
 * ��������� ��
 * �� �� ֵ�� 3- �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2019/06/27	     V1.0	 sdam	      ����
 ***********************************************************************/
__sys__ int do_zoom(void* params)
{


	struct zoom_params *zoom = (struct zoom_params *)params;
	uint8_t *Srcbuff = (uint8_t *)zoom->src_phy;

	/*---------------------------------------test-------------------------------------------------------*/
	trace_err("ptOriginPic buffer0 : %d\n",Srcbuff[0]);								//����arm���͵�����
	trace_err("ptOriginPic buffer1 : %d\n",Srcbuff[1]);								//����arm���͵�����
	trace_err("ptOriginPic buffer2 : %d\n",Srcbuff[2]);								//����arm���͵�����
	trace_err("ptOriginPic Origin width,height: %d:%d\n",zoom->Origin_width,zoom->Origin_height);//����arm���͵�����
	trace_err("ptOriginPic Zoom width,height: %d:%d\n",zoom->Zoom_width,zoom->Zoom_height);//����arm���͵�����
	trace_err("ptOriginPic iBpp : %d\n",zoom->iBpp);									//����arm���͵�����
	/*--------------------------------------------------------------------------------------------------*/
	/*--------------------------------   C����ʵ�֣������ο��������ã�   --------------------------------
	uint32_t dwDstWidth = zoom->Zoom_width;							  						//���ź��ͼ��Ŀ��
	uint32_t *pdwSrcXTable = (uint32_t *)0x2000;											//��������ռ䡣��Ҫ���ڲ��������꣬���ݲ���������ֵ�ɼ�ԭʼͼ�������
	uint32_t x;
	uint32_t y;
	uint32_t i;
	uint32_t dwSrcY;

	uint32_t dwPixelBytes = 3;
	uint32_t Origin_iLineBytes=zoom->Origin_width*dwPixelBytes;
	uint32_t Zoom_iLineBytes=zoom->Zoom_width*dwPixelBytes;

	for (x = 0; x < dwDstWidth; x++)										  						//���ɱ� pdwSrcXTable
	{
		//�������ű���ѡ��ɼ�ԭʼͼ�������㣬��0����ȡԭʼͼ���0���㣬��1����ȡԭʼͼ���1���㣬��2����ȡԭʼͼ���3���㣬��3����ȡԭʼͼ���4���㣬��4����ȡԭʼͼ���6���㣬�Դ����ơ�����
		//pdwSrcXTable[x]=(((uint32_t)(x*3))/2);
		pdwSrcXTable[x]=(((uint32_t)(x*zoom->Origin_width))/zoom->Zoom_width);
	}
	
	for (y = 0; y < zoom->Zoom_height; y++)								  					//���ź�ͼ���������߶�
	{
		dwSrcY = (y * zoom->Origin_height / zoom->Zoom_height);			  	//�������ű���ѡ��ɼ�ԭʼͼ���������Ҳ���ǵڼ������ݣ�
																		  															//��0����ȡԭʼͼ���0���У���1����ȡԭʼͼ���1���У���2����ȡԭʼͼ���3���У��Դ����ơ�����
		//��ָ��ָ��ѹ����ͼ����ڴ�ռ䣬��ָ��һ�еĿ�ʼ��λ�ã�д��֮����һ��ѭ��y++��ָ����һ�п�ʼ��λ��																  
		uint32_t *dst_start = (uint32_t *)zoom->dst_phy  + y * Zoom_iLineBytes;
		//��ָ��ָ��ԭʼͼ����ڴ�ռ䣬��ָ��һ�еĿ�ʼ��λ�ã�д��֮����һ��ѭ��y++ͬ�������ӵ�dwSrcY��ָ����һ�п�ʼ��λ��
		uint32_t *src_start = (uint32_t *)zoom->src_phy  + dwSrcY * Origin_iLineBytes;


		for (x = 0; x <dwDstWidth; x++)										  						//���ź�ͼ��ĺ�����߶�
		{
			 //* ԭͼ����: pdwSrcXTable[x]��srcy
			 //* ��������: x, y
			 //*
			for(i=0;i<3;i++)
			{
				dst_start[x*dwPixelBytes+i]=src_start[pdwSrcXTable[x]*dwPixelBytes+i];
			}
		}
	 }
*/
	return 3;
}



