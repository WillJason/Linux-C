/*
 * Author: SDAM
 * Platform:RV1108
 *	RV1108芯片是瑞芯微布局面向IoT物联网的产品，该芯片内嵌CEVA XM4视觉处理器DSP，具有智能图像处理等关键技术，最高可达600MHz。
 * function:
 *  在ARM平台中打开图片，然后在DSP平台完成采样工作，然后回到ARM平台完成图片的生成。
 * compile:
 * 	makefile选用的是SDK中/externl/dpp/test/Makefile.dsp_ioctl.h也应在在当前的目录。该目录还有dsp_test.cc等test文件
 */

#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "jpeglib.h"

#include <iostream>

#include <sys/mman.h>
#include <ion/ion.h>
#include "dsp_ioctl.h"

#define DMA_1D_TRANSFER  0
#define DMA_2D_TRANSFER  1
#define TEST_MODE        DMA_2D_TRANSFER

struct test_params_arm {
	long unsigned int tmp_phy;
  long unsigned int src_phy;
  long unsigned int dst_phy;

	int tmp_fd;
  int src_fd;
  int dst_fd;

	uint8_t* buf;																											//临时缓冲区，用于存储scanline扫描一行的数据
  uint8_t* src;
  uint8_t* dst;

	ion_user_handle_t tmp_hdl;
  ion_user_handle_t src_hdl;
  ion_user_handle_t dst_hdl;

  uint32_t TempBuffsize;																						//临时缓冲区大小
  uint32_t Originofsize;
  uint32_t DestinOfsize;
};

struct test_params_dsp {
  uint32_t src_phy;
  uint32_t dst_phy;

  uint32_t Origin_width;
  uint32_t Origin_height;

  uint32_t Zoom_width;
  uint32_t Zoom_height;

	uint32_t iBpp;																										//一个像素用多少位表示
  uint32_t mode;
  uint32_t cost_cycles; // Return by DSP.
};

static int g_ion_client = -1;

static int prepare_test_params(struct test_params_arm *params, uint32_t TempBuffsize,uint32_t Originofsize,uint32_t DestinOfsize) {
  int ret = -1;

  memset(params, 0, sizeof(*params));
  params->tmp_hdl = -1;
  params->src_hdl = -1;
  params->dst_hdl = -1;
  params->tmp_fd = -1;
  params->src_fd = -1;
  params->dst_fd = -1;

	ret = ion_alloc(g_ion_client, TempBuffsize, 0,
                  ION_HEAP_TYPE_DMA_MASK, 0, &params->tmp_hdl);
  if (ret) {
    printf("Cannot alloc test temp buffer.\n");
    return ret;
  }

  ret = ion_alloc(g_ion_client, Originofsize, 0,
                  ION_HEAP_TYPE_DMA_MASK, 0, &params->src_hdl);
  if (ret) {
    printf("Cannot alloc test src buffer.\n");
    return ret;
  }

  ret = ion_alloc(g_ion_client, DestinOfsize, 0,
                  ION_HEAP_TYPE_DMA_MASK, 0, &params->dst_hdl);
  if (ret) {
    printf("Cannot alloc test dst buffer.\n");
    return ret;
  }

	ion_get_phys(g_ion_client, params->tmp_hdl, &params->tmp_phy);
  ion_get_phys(g_ion_client, params->src_hdl, &params->src_phy);
  ion_get_phys(g_ion_client, params->dst_hdl, &params->dst_phy);

	ret = ion_share(g_ion_client, params->tmp_hdl, &params->tmp_fd);
  if (ret) {
    printf("Share ion failed.\n");
    return -1;
  }
	
  ret = ion_share(g_ion_client, params->src_hdl, &params->src_fd);
  if (ret) {
    printf("Share ion failed.\n");
    return -1;
  }

  ret = ion_share(g_ion_client, params->dst_hdl, &params->dst_fd);
  if (ret) {
    printf("Share ion failed.\n");
    return -1;
  }

	params->buf = (uint8_t*)mmap(NULL, TempBuffsize, PROT_READ | PROT_WRITE,
                               MAP_SHARED | MAP_LOCKED, params->tmp_fd, 0);
  params->src = (uint8_t*)mmap(NULL, Originofsize, PROT_READ | PROT_WRITE,
                               MAP_SHARED | MAP_LOCKED, params->src_fd, 0);
  params->dst = (uint8_t*)mmap(NULL, DestinOfsize, PROT_READ | PROT_WRITE,
                               MAP_SHARED | MAP_LOCKED, params->dst_fd, 0);

	memset(params->buf, 0, TempBuffsize);
  memset(params->src, 0, Originofsize);
  memset(params->dst, 0, DestinOfsize);

  params->TempBuffsize = TempBuffsize;
  params->Originofsize = Originofsize;
  params->DestinOfsize = DestinOfsize;

  return 0;
}

static int unprepare_test_params(struct test_params_arm *params) {
  if (params->buf)
    munmap(params->buf, params->TempBuffsize);
  if (params->src)
    munmap(params->src, params->Originofsize);
  if (params->dst)
    munmap(params->dst, params->DestinOfsize);

	if (params->tmp_fd >= 0)
    close(params->tmp_fd);
  if (params->src_fd >= 0)
    close(params->src_fd);
  if (params->dst_fd >= 0)
    close(params->dst_fd);

	if (params->tmp_hdl >= 0) {
    ion_free(g_ion_client, params->tmp_hdl);
    close(params->tmp_hdl);
  }
  if (params->src_hdl >= 0) {
    ion_free(g_ion_client, params->src_hdl);
    close(params->src_hdl);
  }
  if (params->dst_hdl >= 0) {
    ion_free(g_ion_client, params->dst_hdl);
    close(params->dst_hdl);
  }
  return 0;
}

void mkjpeg(int width,int height,uint8_t *buffer)
{
	
}

int main(int argc, char **argv) {
  int dsp_fd = -1;
  
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  FILE * infile;
  FILE * fp;
  int origin_width = 2560;																					//原始图像宽度
  int origin_height = 1440;																					//原始图像高度
  int OriginPtsize = origin_width * origin_height;
  int zoom_width = 1366;																						//缩放后宽度
  int zoom_height = 768;																						//缩放后高度
  int row_stride;
  
  int iTotalBytes;
  int i;
  int zoom_stride;
  int zoomiTotalBytes;  
  int Libjpeg_iBpp = 24;																						//一个像素用24位表示，libjpeg解压出来的图像数据为24位RGB格式
  int testi,testj;
  
  uint8_t* tmpbuff;
  uint8_t* Srcbuff;
  uint8_t* testbuff;
  
  
  struct jpeg_compress_struct jcs;																	//create piture test
	struct jpeg_error_mgr jem;																				//create piture test
	FILE *fp_create;																									//create piture test
	JSAMPROW row_pointer[1];																					//一行位图
	int create_stride;																								//每一行的字节数
  
  struct dsp_user_work test_work;
  struct test_params_arm params_arm;
  struct test_params_dsp params_dsp;

  memset(&test_work, 0, sizeof(test_work));
  memset(&params_dsp, 0, sizeof(params_dsp));

  g_ion_client = ion_open();
  if (g_ion_client < 0) {
    printf("Cannot open ion device.\n");
    return -1;
  }

  // Open DSP device.
  dsp_fd = open("/dev/dsp", O_RDWR);
  if (dsp_fd < 0) {
    printf("Cannot open dsp device.\n");
    return -1;
  }

/*--------------------开始对图像进行操作，这里使用的libjpeg库对jpg图片进行操作---------------------------------*/
  if(argc != 2)
  {
    printf("Usage:\n");
    return -1;
  }  
  cinfo.err = jpeg_std_error(&jerr);																// 分配和初始化一个decompression结构体
  jpeg_create_decompress(&cinfo);
  
  if((infile = fopen(argv[1],"rb"))==NULL)													// 指定源文件
  {
    printf("cant open");
    return -1; 
  }
  jpeg_stdio_src(&cinfo,infile);
  
  jpeg_read_header(&cinfo,TRUE);																		// 用jpeg_read_header获得jpg信息

  cinfo.scale_num=1;																								//重点在采样，所以采用原始大小读取
	cinfo.scale_denom=1;
  printf("1:1 ratio to extract the original image information and data.\n"); //1:1比例取出原始的图像信息以及数据
	
  jpeg_start_decompress(&cinfo);                                 		// 开始解压缩，在完成解压缩操作后，IJG就会将解压后的图像信息填充至cinfo结构中。

  /* -----------------------------------原始的以及压缩后的图象信息 -------------------------------------------*/
  printf("Origin_width = %d,Origin_height = %d,Origin_components = %d\n", cinfo.output_width, cinfo.output_height, cinfo.output_components);
  printf("If compressed, the image data obtained are as follows:\n");
  printf("Dest_width = %d,Dest_height = %d,Dest_components = %d\n", zoom_width, zoom_height, cinfo.output_components);

	/*------------------------------------计算大小以及分配内存--------------------------------------------------*/
	row_stride = cinfo.output_width * cinfo.output_components;		    //原始图像一行的数据长度
	iTotalBytes = row_stride*cinfo.output_height;											//计算原始图像的大小
 	zoom_stride=zoom_width*cinfo.output_components;										//计算压缩后的图像一行数据的长度，有多少个字节
  zoomiTotalBytes = 	zoom_stride*zoom_height;											//计算压缩后的图像的大小
  /*--------------------------------------------------------------------------------------------------*/
  // Initialize work struct.
  //static int prepare_test_params(struct test_params_arm *params, uint32_t TempBuffsize,uint32_t Originofsize,uint32_t DestinOfsize)
  if (prepare_test_params(&params_arm,row_stride,iTotalBytes,zoomiTotalBytes)) {
    unprepare_test_params(&params_arm);
    printf("Prepare test parameter failed.\n");
    return -1;
  }
  /*----------------------------------------------------------------------------------------------------------
	*		循环调用jpeg_read_scanlines来一行一行地获得解压的数据，解开的数据是按照行取出的，数据像素按照scanline来
	*存储，scanline是从左到右，从上到下的顺序，每个像素对应的各颜色或灰度通道数据是依次存储
	-------------------------------------------------------------------------------------------------------*/
//	Srcbuff = params_arm.src;
	while (cinfo.output_scanline < cinfo.output_height) 
	{
		(void) jpeg_read_scanlines(&cinfo, &params_arm.buf, 1);	
    memcpy(params_arm.src+(uint32_t)((cinfo.output_scanline-1)*row_stride),params_arm.buf, row_stride);                
	}
  
  params_dsp.src_phy = params_arm.src_phy;
  params_dsp.dst_phy = params_arm.dst_phy;
  params_dsp.mode = TEST_MODE;
  params_dsp.iBpp = Libjpeg_iBpp;
  params_dsp.Origin_width = origin_width;
  params_dsp.Origin_height = origin_height;
  params_dsp.Zoom_width = zoom_width;
  params_dsp.Zoom_height = zoom_height;  

  test_work.magic = DSP_ALGORITHM_WORK_MAGIC;
  test_work.id = 0x78787878;
  test_work.algorithm.type = DSP_ALGORITHM_ZOOM;
  test_work.algorithm.packet_virt = (u32)&params_dsp;
  test_work.algorithm.size = sizeof(params_dsp);

  // Request DSP work. The test algorithm just copy src buffer to dst buffer
  // using DDMA interface.
  printf("Test work start, mode=%d, src=0x%08x, dst=0x%08x.\n",
         params_dsp.mode, params_dsp.src_phy, params_dsp.dst_phy);
  /*----------------------------------------test测试用----------------------------------------------*/
  Srcbuff = params_arm.src;
  //for(testi=40;testi<56;testi++)
  //{
  //	printf("Test work start, RGB%d =%d:%d:%d.\n",testi,Srcbuff[testi*3],Srcbuff[testi*3+1],Srcbuff[testi*3+2]);
  //}
  
  /*----------------------------------------------------------------------------------------*/
  //DSP_IOC_QUEUE_WORK：传递工作参数 struct dsp_user_work 给 DSP，请求 DSP 调用适合的算法进行运算
  ioctl(dsp_fd, DSP_IOC_QUEUE_WORK, &test_work);	
	
  // Process result, check buffer copy operation is success or not.
  //DSP_IOC_DEQUEUE_WORK：获取 DSP 的计算结果，此接口为阻塞接口，会在 kernel 中等到 DSP 工作完成
  ioctl(dsp_fd, DSP_IOC_DEQUEUE_WORK, &test_work);

	/*----------------------------------------test测试用----------------------------------------------*/
  testbuff = params_arm.dst;
  //for(testj=27;testj<38;testj++)
  //{
  //	printf("Test DSP, RGB%d =%d:%d:%d.\n",testj,testbuff[testj*3],testbuff[testj*3+1],testbuff[testj*3+2]);
  //}
  /*----------------------------------------test----------------------------------------------*/
  printf("Test success.\n");
  
  /*------------------------------create piture------------------------------------------------*/
  
	
	jcs.err = jpeg_std_error(&jem);
	jpeg_create_compress(&jcs);
	
	fp_create = fopen("new.jpeg","wb");
	
	jpeg_stdio_dest(&jcs,fp_create);
	
	jcs.image_width = zoom_width;																			//位图的宽和高，单位为像素
	jcs.image_height = zoom_height;
	jcs.input_components = 3;																					//在此为3,表示为彩图 如果是灰度图，则为1
	jcs.in_color_space = JCS_RGB;																			//JCS_RGB表示彩色图,JCS_GRAYSCALE表示灰度图
	create_stride = zoom_width * 3;																		//一行像素占的字节数
	
	jpeg_set_defaults(&jcs);
	jpeg_set_quality(&jcs,80,TRUE);
	jpeg_start_compress(&jcs,TRUE);
	while(jcs.next_scanline < jcs.image_height)
	{
		row_pointer[0]=&testbuff[jcs.next_scanline * create_stride];
		jpeg_write_scanlines(&jcs,row_pointer,1);
	}
	jpeg_finish_compress(&jcs);
	jpeg_destroy_compress(&jcs);
	fclose(fp_create);
  /*-----------------------------------------------------------------------------------------------*/

  unprepare_test_params(&params_arm);
  close(dsp_fd);
  close(g_ion_client);
  return 0;
}
