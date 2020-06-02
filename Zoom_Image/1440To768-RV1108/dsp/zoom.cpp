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

/* 图片的象素数据 */
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
 * 函数名称： PicZoom
 * 功能描述： 近邻取样插值方法缩放图片
 *            注意该函数会分配内存来存放缩放后的图片,用完后要用free函数释放掉
 *            "近邻取样插值"的原理请参考所著的"图像缩放算法"
 * 输入参数： 	ptOriginPic - 内含原始图片的象素数据
 *          ptZoomPic    - 内含缩放后的图片的象素数据
 * 输出参数： 无
 * 返 回 值： 3- 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2019/06/27	     V1.0	 sdam	      创建
 ***********************************************************************/
__sys__ int do_zoom(void* params)
{


	struct zoom_params *zoom = (struct zoom_params *)params;
	uint8_t *Srcbuff = (uint8_t *)zoom->src_phy;

	/*---------------------------------------test-------------------------------------------------------*/
	trace_err("ptOriginPic buffer0 : %d\n",Srcbuff[0]);								//测试arm传送的数据
	trace_err("ptOriginPic buffer1 : %d\n",Srcbuff[1]);								//测试arm传送的数据
	trace_err("ptOriginPic buffer2 : %d\n",Srcbuff[2]);								//测试arm传送的数据
	trace_err("ptOriginPic Origin width,height: %d:%d\n",zoom->Origin_width,zoom->Origin_height);//测试arm传送的数据
	trace_err("ptOriginPic Zoom width,height: %d:%d\n",zoom->Zoom_width,zoom->Zoom_height);//测试arm传送的数据
	trace_err("ptOriginPic iBpp : %d\n",zoom->iBpp);									//测试arm传送的数据
	/*--------------------------------------------------------------------------------------------------*/
	/*--------------------------------   C方法实现（仅供参考，不可用）   --------------------------------
	uint32_t dwDstWidth = zoom->Zoom_width;							  						//缩放后的图像的宽度
	uint32_t *pdwSrcXTable = (uint32_t *)0x2000;											//分配采样空间。主要用于采样点坐标，根据采样点坐标值采集原始图像的数据
	uint32_t x;
	uint32_t y;
	uint32_t i;
	uint32_t dwSrcY;

	uint32_t dwPixelBytes = 3;
	uint32_t Origin_iLineBytes=zoom->Origin_width*dwPixelBytes;
	uint32_t Zoom_iLineBytes=zoom->Zoom_width*dwPixelBytes;

	for (x = 0; x < dwDstWidth; x++)										  						//生成表 pdwSrcXTable
	{
		//根据缩放比例选择采集原始图像的坐标点，第0个点取原始图像第0个点，第1个点取原始图像第1个点，第2个点取原始图像第3个点，第3个点取原始图像第4个点，第4个点取原始图像第6个点，以此类推。。。
		//pdwSrcXTable[x]=(((uint32_t)(x*3))/2);
		pdwSrcXTable[x]=(((uint32_t)(x*zoom->Origin_width))/zoom->Zoom_width);
	}
	
	for (y = 0; y < zoom->Zoom_height; y++)								  					//缩放后图像的纵坐标高度
	{
		dwSrcY = (y * zoom->Origin_height / zoom->Zoom_height);			  	//根据缩放比例选择采集原始图像的纵坐标也就是第几行数据，
																		  															//第0个行取原始图像第0个行，第1个行取原始图像第1个行，第2个行取原始图像第3个行，以此类推。。。
		//将指针指向压缩后图像的内存空间，并指向一行的开始的位置，写完之后下一次循环y++就指向下一行开始的位置																  
		uint32_t *dst_start = (uint32_t *)zoom->dst_phy  + y * Zoom_iLineBytes;
		//将指针指向原始图像的内存空间，并指向一行的开始的位置，写完之后下一次循环y++同比例增加的dwSrcY就指向下一行开始的位置
		uint32_t *src_start = (uint32_t *)zoom->src_phy  + dwSrcY * Origin_iLineBytes;


		for (x = 0; x <dwDstWidth; x++)										  						//缩放后图像的横坐标高度
		{
			 //* 原图座标: pdwSrcXTable[x]，srcy
			 //* 缩放座标: x, y
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



