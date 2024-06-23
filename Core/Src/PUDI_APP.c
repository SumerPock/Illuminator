
#include "bsp.h"

extern SPI_HandleTypeDef hspi2;
//extern UART_HandleTypeDef huart1;
//extern DAC_HandleTypeDef hdac;
const unsigned char TAD2141CRC[256] =
{
		0x00,0x1D,0x3A,0x27,0x74,0x69,0x4E,0x53,0xE8,0xF5,0xD2,0xCF,0x9C,0x81,0xA6,0xBB,
		0xCD,0xD0,0xF7,0xEA,0xB9,0xA4,0x83,0x9E,0x25,0x38,0x1F,0x02,0x51,0x4C,0x6B,0x76,
		0x87,0x9A,0xBD,0xA0,0xF3,0xEE,0xC9,0xD4,0x6F,0x72,0x55,0x48,0x1B,0x06,0x21,0x3C,
		0x4A,0x57,0x70,0x6D,0x3E,0x23,0x04,0x19,0xA2,0xBF,0x98,0x85,0xD6,0xCB,0xEC,0xF1,
		0x13,0x0E,0x29,0x34,0x67,0x7A,0x5D,0x40,0xFB,0xE6,0xC1,0xDC,0x8F,0x92,0xB5,0xA8,
		0xDE,0xC3,0xE4,0xF9,0xAA,0xB7,0x90,0x8D,0x36,0x2B,0x0C,0x11,0x42,0x5F,0x78,0x65,
		0x94,0x89,0xAE,0xB3,0xE0,0xFD,0xDA,0xC7,0x7C,0x61,0x46,0x5B,0x08,0x15,0x32,0x2F,
		0x59,0x44,0x63,0x7E,0x2D,0x30,0x17,0x0A,0xB1,0xAC,0x8B,0x96,0xC5,0xD8,0xFF,0xE2,
		0x26,0x3B,0x1C,0x01,0x52,0x4F,0x68,0x75,0xCE,0xD3,0xF4,0xE9,0xBA,0xA7,0x80,0x9D,
		0xEB,0xF6,0xD1,0xCC,0x9F,0x82,0xA5,0xB8,0x03,0x1E,0x39,0x24,0x77,0x6A,0x4D,0x50,
		0xA1,0xBC,0x9B,0x86,0xD5,0xC8,0xEF,0xF2,0x49,0x54,0x73,0x6E,0x3D,0x20,0x07,0x1A,
		0x6C,0x71,0x56,0x4B,0x18,0x05,0x22,0x3F,0x84,0x99,0xBE,0xA3,0xF0,0xED,0xCA,0xD7,
		0x35,0x28,0x0F,0x12,0x41,0x5C,0x7B,0x66,0xDD,0xC0,0xE7,0xFA,0xA9,0xB4,0x93,0x8E,
		0xF8,0xE5,0xC2,0xDF,0x8C,0x91,0xB6,0xAB,0x10,0x0D,0x2A,0x37,0x64,0x79,0x5E,0x43,
		0xB2,0xAF,0x88,0x95,0xC6,0xDB,0xFC,0xE1,0x5A,0x47,0x60,0x7D,0x2E,0x33,0x14,0x09,
		0x7F,0x62,0x45,0x58,0x0B,0x16,0x31,0x2C,0x97,0x8A,0xAD,0xB0,0xE3,0xFE,0xD9,0xC4
};

/************************************************************************************************************
 * Name    : fputc
 * Func    : re werite printf function
 * input   : 
 * output  : 
 * Author  : 
 * history : 
*************************************************************************************************************/
int fputc(int ch, FILE *f)  
{  	
//	uint8_t temp[1]={ch};
//  HAL_UART_Transmit(&huart1, (uint8_t *)&temp ,1,2);
  return ch;
}
/************************************************************************************************************
 * Name    : fgetc
 * Func    : re werite scanf function
 * input   : 
 * output  : 
 * Author  : 
 * history : 
*************************************************************************************************************/
int fgetc(FILE *f)   
{
	uint8_t ch;	
//	HAL_UART_Receive(&huart1, (uint8_t *)&ch, sizeof(ch), 0xFFFF);	
	return ch;
}
/************************************************************************************************************
 * Name    : PUDI_CRC8
 * Func    : Table CRC is the pointer to the look-up table (LUT)
 * input   : 
 * output  : 
 * Author  : 
 * history : 
*************************************************************************************************************/
//*Table CRC is the pointer to the look-up table (LUT)
unsigned char PUDI_CRC8(unsigned char *message, unsigned char Bytelength, const unsigned char* TableCRC) 
{
		unsigned char crc;
		int Byteidx;

		crc = 0xFF;
		for(Byteidx=0; Byteidx<Bytelength; Byteidx++)
		{
			crc = TableCRC[crc ^ *(message+Byteidx)];
		}
		return(~crc);
}
/************************************************************************************************************
 * Name    : ASIC_VDD_PowerCtrl
 * Func    : Power on encoder
 * input   : 
 * output  : 
 * Author  : 
 * history : 
*************************************************************************************************************/

unsigned char PUDI_PowerCtrl(double Voltage) 
{

//		HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (Voltage/4.4/3.3)*4096);

//		HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
		return 1;
}
/************************************************************************************************************
 * Name    : PUDI_InitSPI
 * Func    : SPI initial
 * input   : 
 * output  : 
 * Author  : 
 * history : 
*************************************************************************************************************/
void DelayUs(uint32_t udelay)
{
  uint32_t startval,tickn,delays,wait;

  startval = SysTick->VAL;
  tickn = HAL_GetTick();
  delays =udelay * 168; 
  if(delays > startval)
  {
      while(HAL_GetTick() == tickn);

      wait = 72000 + startval - delays;
      while(wait < SysTick->VAL);
  }
  else
  {
      wait = startval - delays;
      while(wait < SysTick->VAL && HAL_GetTick() == tickn);
  }
}

/****************************************************************************
 * Name    : PUDI_ReadAngle
 * Func    : Read encoder angle data include angle status and CRC check
 * input   : 
 * output  : Status crc  angle
 * Author  : 
 * history : 
*****************************************************************************/
uint8_t PUDI_ReadAngle(uint8_t *state,uint8_t *CRC8, int32_t* ReadRegData)  
{	
	  uint8_t  crc8,SendSpi;
	  
		PUDIANGLEFRAME SPIAngle;   
		uint8_t * rddata = (uint8_t*)&SPIAngle; 
    uint8_t retry = 0;
	
  	SPI1_CS_LOW;     //bit[0]: Module _spi_ena  
		DelayUs(1);

		SendSpi = 0x05;

		if(HAL_SPI_Transmit(&hspi2, &SendSpi, 1, SPI_TIMEOUT_MAX) != HAL_OK) //SPI write first command data.
			return 0; 
		
		for(retry =0; retry <6; retry++)
		{
				if(HAL_SPI_Receive(&hspi2, (uint8_t*)&SPIAngle, 4, SPI_TIMEOUT_MAX) != HAL_OK)
						return 0;	

				crc8 = PUDI_CRC8(rddata,3,TAD2141CRC);
				if(SPIAngle.crc8 == crc8)
				{
					*ReadRegData = ((SPIAngle.angle>>8)&0x00ff) | (SPIAngle.angle<<8 & 0xff00);
					*state = SPIAngle.state;  
					*CRC8 =	SPIAngle.crc8;				
					DelayUs(1);
					SPI1_CS_HIGH;     //bit[0]: Disable SPI function 
					return 1;
				}
		}	
		DelayUs(1);
		SPI1_CS_HIGH;     //bit[0]: Disable SPI function 
		return 0;
}
/*************************************************************************************
 * Name    :  App_TaskPUDIDemo
 * Func    :  编码器功能测试6000次，读取角度，计算圈数，并通过串口打印出来。
 * input   :  
 * output  : 
 * Author  : 
 * history : 
***************************************************************************************/
//static PUDI_ENCODERFUNC CfgPrm;

//void App_TaskPUDIDemo(void)
//{
//	
//	uint8_t CRCFlag = 0;
//	uint16_t bk = 1,test = 0;
//	int32_t SpiAngle = 0,PrvSpiAngle = 0; 
//	uint8_t StreamStatus=0,StreamCRC=0;	
//	int CountFlag;
//		
//	float spiangle= 0.0;
//	

//	PUDI_PowerCtrl(5.0);		//上电控制，客户应用不需要，电源直接供电
//	
//	SPI1_CS_HIGH;     //bit[0]: Disable SPI function			
//	HAL_Delay(100);		//上电之后必须等待超过20ms，编码器才完全上电.
//	
//	CfgPrm.Counter = 0;//初始化计圈参数
//	PUDI_ReadAngle(&StreamStatus,&StreamCRC,&PrvSpiAngle);	

//	while (1)
//	{
//		
//		if(test<6000)		//只测试6000次
//		{
//			test++;

//			bk = PUDI_ReadAngle(&StreamStatus,&StreamCRC,&SpiAngle);	
//			if(bk == 0) continue;		//CRC校验错误，重复读取
//			
//			CountFlag = SpiAngle - PrvSpiAngle;
//			if(abs(SpiAngle - PrvSpiAngle) > HystLSB) 	//增加迟滞判断 定义5bit LSB，拟制抖动干扰
//					PrvSpiAngle = SpiAngle;

//			if(CountFlag > 16384)		//角度翻转范围为0~65536 或者 65536~0 增加阈值90° 拟制干扰
//					CfgPrm.Counter++;
//			else if(-CountFlag > 16384)
//					CfgPrm.Counter--;
//			spiangle = (double)SpiAngle/65536.0*360.0;	

//			printf("OK,Counter %x,Angle %f deg,STUS %x,CRC %x,CRCerror %x\r\n",CfgPrm.Counter,spiangle,StreamStatus,StreamCRC,CRCFlag);
//		}
//	
//		HAL_Delay(10);		//系统测试延时函数，请调用自己平台的延时
//	}
//}
///*********************************************************************************************************/
