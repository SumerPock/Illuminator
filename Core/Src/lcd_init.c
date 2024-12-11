#include "bsp.h"


/*
*********************************************************************************************************
*	函 数 名: 
*	功能说明: 
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_GPIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	
	GPIO_InitStruct.Pin       = GPIO_PIN_0;
	GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);	
	
	GPIO_InitStruct.Pin       = GPIO_PIN_13 | GPIO_PIN_15;
	GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin       = GPIO_PIN_7;
	GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
}


/*
*********************************************************************************************************
*	函 数 名: 
*	功能说明: 
*	形    参: 
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_Clear(uint16_t Color)
{
	uint16_t i , j;  	
	LCD_Address_Set(0 , 0 , LCD_W - 1 , LCD_H - 1);
	for(i = 0 ; i <= LCD_W ; i++)
	{
		for (j = 0 ; j <= LCD_H ; j++)
		{
			LCD_WR_DATA(Color);	 			 
		}
	}
}

void LCD_ClearArea(uint16_t xbegin , uint16_t ybegin , uint16_t xend , uint16_t yend , uint16_t Color)
{
	uint16_t i = xbegin , j = ybegin;  	
	LCD_Address_Set( xbegin , ybegin , xend , yend);
	for(i = xbegin; i <= xend ; i++)
	{
		for (j = ybegin; j <= yend ; j++)
		{
			LCD_WR_DATA(Color);	 			 
		}
	}
}

/*
*********************************************************************************************************
*	函 数 名: 
*	功能说明: LCD串行数据写入函数
*	形    参: dat  要写入的串行数据
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_Writ_Bus(unsigned char dat) 
{	
	LCD_CS_Clr();
	if(HAL_SPI_Transmit(&hspi , &dat , 1 , SPITIMEOUT) != HAL_OK)
	{
		printf("hspi send data error");
	}
	LCD_CS_Set();
}


/*
*********************************************************************************************************
*	函 数 名: 
*	功能说明: LCD写入数据
*	形    参: dat 写入的数据
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_WR_DATA8(unsigned char dat)
{
	LCD_Writ_Bus(dat);
}


/*
*********************************************************************************************************
*	函 数 名: 
*	功能说明: LCD写入数据
*	形    参: dat 写入的数据
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_WR_DATA(unsigned short dat)
{
	LCD_CS_Clr();
	LCD_DC_Set();//写数据
	unsigned char ucdat[2] = {0};
	ucdat[1] = dat;
	ucdat[0] = dat >> 8;
	if(HAL_SPI_Transmit(&hspi , ucdat , 2 , SPITIMEOUT) != HAL_OK)
	{
		printf("hspi send data error");
	}
	LCD_CS_Set();
}

/*
*********************************************************************************************************
*	函 数 名: 
*	功能说明: LCD写入命令
*	形    参: dat 写入的命令
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_WR_REG(unsigned char dat)
{
	LCD_DC_Clr();//写命令
	LCD_Writ_Bus(dat);
	LCD_DC_Set();//写数据
}


/*
*********************************************************************************************************
*	函 数 名: 
*	功能说明: 设置起始和结束地址
*	形    参: x1,x2 设置列的起始和结束地址 , y1,y2 设置行的起始和结束地址
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_Address_Set(unsigned short x1,unsigned short y1,unsigned short x2,unsigned short y2)
{
	if(USE_HORIZONTAL == 0)
	{
		LCD_WR_REG(0x2a);//列地址设置
		LCD_WR_DATA(x1);
		LCD_WR_DATA(x2);
		LCD_WR_REG(0x2b);//行地址设置
		LCD_WR_DATA(y1);
		LCD_WR_DATA(y2);
		LCD_WR_REG(0x2c);//储存器写
	}
	else if(USE_HORIZONTAL==1)
	{
		LCD_WR_REG(0x2a);//列地址设置
		LCD_WR_DATA(x1);
		LCD_WR_DATA(x2);
		LCD_WR_REG(0x2b);//行地址设置
		LCD_WR_DATA(y1);
		LCD_WR_DATA(y2);
		LCD_WR_REG(0x2c);//储存器写
	}
	else if(USE_HORIZONTAL==2)
	{
		LCD_WR_REG(0x2a);//列地址设置
		LCD_WR_DATA(x1);
		LCD_WR_DATA(x2);
		LCD_WR_REG(0x2b);//行地址设置
		LCD_WR_DATA(y1);
		LCD_WR_DATA(y2);
		LCD_WR_REG(0x2c);//储存器写
	}
	else
	{
		LCD_WR_REG(0x2a);//列地址设置
		LCD_WR_DATA(x1);
		LCD_WR_DATA(x2);
		LCD_WR_REG(0x2b);//行地址设置
		LCD_WR_DATA(y1);
		LCD_WR_DATA(y2);
		LCD_WR_REG(0x2c);//储存器写
	}
}


/*
*********************************************************************************************************
*	函 数 名: 
*	功能说明: 
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_Init(void)
{
	//SPI1_Init();
	LCD_GPIO_Init();
	LCD_RES_Clr();//复位
	HAL_Delay(10);	
	LCD_RES_Set();
	HAL_Delay(10);
	LCD_BLK_Set();//打开背光
	HAL_Delay(10);
	LCD_WR_REG(0xCF);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0xC1);
	LCD_WR_DATA8(0X30);
	LCD_WR_REG(0xED);
	LCD_WR_DATA8(0x64);
	LCD_WR_DATA8(0x03);
	LCD_WR_DATA8(0X12);
	LCD_WR_DATA8(0X81);
	LCD_WR_REG(0xE8);
	LCD_WR_DATA8(0x85);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x79);
	LCD_WR_REG(0xCB);
	LCD_WR_DATA8(0x39);
	LCD_WR_DATA8(0x2C);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x34);
	LCD_WR_DATA8(0x02);
	LCD_WR_REG(0xF7);
	LCD_WR_DATA8(0x20);
	LCD_WR_REG(0xEA);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x00);
	LCD_WR_REG(0xC0); //Power control
	LCD_WR_DATA8(0x1D); //VRH[5:0]
	LCD_WR_REG(0xC1); //Power control
	LCD_WR_DATA8(0x12); //SAP[2:0];BT[3:0]
	LCD_WR_REG(0xC5); //VCM control
	LCD_WR_DATA8(0x33);
	LCD_WR_DATA8(0x3F);
	LCD_WR_REG(0xC7); //VCM control
	LCD_WR_DATA8(0x92);
	LCD_WR_REG(0x3A); // Memory Access Control
	LCD_WR_DATA8(0x55);
	LCD_WR_REG(0x36); // Memory Access Control

	if(USE_HORIZONTAL==0)
	{
		LCD_WR_DATA8(0x08);
	}
	else if(USE_HORIZONTAL==1)
	{
		LCD_WR_DATA8(0xC8);
	}
	else if(USE_HORIZONTAL==2)
	{
		LCD_WR_DATA8(0x78);
	}
	else 
	{
		LCD_WR_DATA8(0xA8);
	}
		
	LCD_WR_REG(0xB1);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x12);
	LCD_WR_REG(0xB6); // Display Function Control
	LCD_WR_DATA8(0x0A);
	LCD_WR_DATA8(0xA2);
	
	LCD_WR_REG(0x44);
	LCD_WR_DATA8(0x02);
	
	LCD_WR_REG(0xF2); // 3Gamma Function Disable
	LCD_WR_DATA8(0x00);
	LCD_WR_REG(0x26); //Gamma curve selected
	LCD_WR_DATA8(0x01);
	LCD_WR_REG(0xE0); //Set Gamma
	LCD_WR_DATA8(0x0F);
	LCD_WR_DATA8(0x22);
	LCD_WR_DATA8(0x1C);
	LCD_WR_DATA8(0x1B);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x0F);
	LCD_WR_DATA8(0x48);
	LCD_WR_DATA8(0xB8);
	LCD_WR_DATA8(0x34);
	LCD_WR_DATA8(0x05);
	LCD_WR_DATA8(0x0C);
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x0F);
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x00);
	LCD_WR_REG(0XE1); //Set Gamma
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x23);
	LCD_WR_DATA8(0x24);
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x10);
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x38);
	LCD_WR_DATA8(0x47);
	LCD_WR_DATA8(0x4B);
	LCD_WR_DATA8(0x0A);
	LCD_WR_DATA8(0x13);
	LCD_WR_DATA8(0x06);
	LCD_WR_DATA8(0x30);
	LCD_WR_DATA8(0x38);
	LCD_WR_DATA8(0x0F);
	LCD_WR_REG(0x11); 	//Exit Sleep
	HAL_Delay(5);				//delay_ms(120);
	LCD_WR_REG(0x29); 	//Display on
} 

