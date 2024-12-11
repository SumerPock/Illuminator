#ifndef __LCD_INIT_H
#define __LCD_INIT_H

//#include "sys.h"
#define USE_HORIZONTAL 0  //设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏


#if USE_HORIZONTAL == 0 || USE_HORIZONTAL == 1
#define LCD_W 240
#define LCD_H 320

#else
#define LCD_W 320
#define LCD_H 240
#endif

//-----------------LCD端口定义---------------- 
/**************************************
// 这部分说明的管脚映射和董哲发来的程序不同，特请留意！！！
SCL --- LCD.3 -- PB13 -- SPI_SCK   	硬件SPI控制
SDA --- LCD.4 -- PB15 -- SPI_MOSI		硬件SPI控制

RES --- LCD.5 -- PA0 -- LCD_RESET		重置
DC  --- LCD.6 -- PC13 -- LCD_DC			
CS  --- LCD.7 -- PC15 -- SPI_NSS		好像没怎么用
BLK --- LCD.8 -- PE7 -- LCD_BLK			背光
**************************************/

/*背光*/
#define LCD_BLK_Clr()  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_RESET)
#define LCD_BLK_Set()  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_SET)

/*重置*/
#define LCD_RES_Clr()  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET)//RES
#define LCD_RES_Set()  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET)


#define LCD_DC_Clr()   HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET)//DC
#define LCD_DC_Set()   HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET)
 		     
#define LCD_CS_Clr()   HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_RESET)//CS
#define LCD_CS_Set()   HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_SET)


void LCD_ClearArea(uint16_t xbegin , uint16_t ybegin , uint16_t xend , uint16_t yend , uint16_t Color);
void LCD_Clear(uint16_t Color);
void LCD_GPIO_Init(void);							
void LCD_Writ_Bus(unsigned char dat);		
void LCD_WR_DATA8(unsigned char dat);		
void LCD_WR_DATA(unsigned short dat);		
void LCD_WR_REG(unsigned char dat);			
void LCD_Address_Set(unsigned short x1,unsigned short y1,unsigned short x2,unsigned short y2);
void LCD_Init(void);
#endif