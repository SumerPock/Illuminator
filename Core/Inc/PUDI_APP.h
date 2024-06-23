#ifndef _PUDI_APP_H
#define _PUDI_APP_H

#include "stdint.h"
#include "stm32f4xx_hal.h"

#define  SPI1_CS_LOW			HAL_GPIO_WritePin(GPIOB , GPIO_PIN_9 , GPIO_PIN_RESET)                                  
#define  SPI1_CS_HIGH			HAL_GPIO_WritePin(GPIOB , GPIO_PIN_9 , GPIO_PIN_SET)

#define SPI_TIMEOUT_MAX           0x1000

#define FALSE  	0
#define TRUE  	1

//#define  SPI_RX_DMA_CH		DMA1_Channel2                                  
//#define  SPI_TX_DMA_CH		DMA1_Channel3

#define HystLSB (0x01<<5)

typedef struct PUDI_AngleFrame
{
  uint16_t angle;  
	uint8_t state;
	uint8_t crc8;

} PUDIANGLEFRAME; 


typedef struct PUDI_EncoderFunc
{
  uint16_t AngleZeroSet;  
	uint16_t Counter;
	uint8_t Status;

} PUDI_ENCODERFUNC; 

void  App_TaskPUDIDemo (void);
#endif
