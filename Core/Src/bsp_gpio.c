#include "bsp.h"

/* 控制端口对应的RCC时钟 */
#define GPIO_GPIO_CLK_ALLENABLE() {	  \
		__HAL_RCC_GPIOD_CLK_ENABLE(); \
		__HAL_RCC_GPIOC_CLK_ENABLE(); \
		__HAL_RCC_GPIOA_CLK_ENABLE(); \
	};
/*创建GPIO回调控制函数*/
//1 . 定义俩个函数分别实现GPIO拉高拉低功能


	
/* USER CODE BEGIN 0 */
#define U3Sp330485ENPin			GPIO_PIN_1		
#define U3Sp330422Pin				GPIO_PIN_0
#define U3Sp330SHDNPin 			GPIO_PIN_12
#define U3Sp330SLEWPin 			GPIO_PIN_15
#define U3Sp330485or232Pin 	GPIO_PIN_5	
#define U3Sp330HALEFULLPin 	GPIO_PIN_2

#define U3Sp330485ENPort		GPIOD
#define U3Sp330422Port			GPIOD
#define U3Sp330SHDNPort 		GPIOC
#define U3Sp330SLEWPort 		GPIOA
#define U3Sp330485or232Port GPIOD
#define U3Sp330HALEFULLPort GPIOD

#define U4Sp330485ENPin			GPIO_PIN_4		
#define U4Sp330422Pin				GPIO_PIN_6
#define U4Sp330SHDNPin 			GPIO_PIN_1
#define U4Sp330SLEWPin 			GPIO_PIN_0
#define U4Sp330485or232Pin 	GPIO_PIN_9	
#define U4Sp330HALEFULLPin 	GPIO_PIN_8

#define U4Sp330485ENPort		GPIOB
#define U4Sp330422Port			GPIOD
#define U4Sp330SHDNPort 		GPIOE
#define U4Sp330SLEWPort 		GPIOE
#define U4p330485or232Port 	GPIOB
#define U4Sp330HALEFULLPort GPIOB

//sp330配置
void ConfigSP330(unsigned char num , unsigned char mode)
{
	SP330NUM sn;
	SP330MODE sm;
	sn = (SP330NUM)num;
	sm = (SP330MODE)mode;
	GPIO_InitTypeDef  GPIO_InitStruct;
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	switch(sn)
	{
		case U1:
			GPIO_InitStruct.Pin   = GPIO_PIN_15;
			GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
			GPIO_InitStruct.Pull  = GPIO_PULLUP;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
			HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

			GPIO_InitStruct.Pin   = GPIO_PIN_1 | GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_5;
			GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
			GPIO_InitStruct.Pull  = GPIO_PULLUP;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
			HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);		
		
			GPIO_InitStruct.Pin   = GPIO_PIN_12;
			GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
			GPIO_InitStruct.Pull  = GPIO_PULLUP;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
			HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);	
		
			HAL_GPIO_WritePin(GPIOC , GPIO_PIN_12 , GPIO_PIN_SET); //SHDN 	低电平时启动低功耗
			HAL_GPIO_WritePin(GPIOA , GPIO_PIN_15 , GPIO_PIN_SET); //SLEW		低电平时限制速度250K
			HAL_GPIO_WritePin(GPIOD , GPIO_PIN_1 , GPIO_PIN_RESET);//485_EN	低电平时启动485接收
			HAL_GPIO_WritePin(GPIOD , GPIO_PIN_0 , GPIO_PIN_SET);  //T2IN, DE 高电平时启动驱动程序
			switch(sm)
			{
				case RS232:
					HAL_GPIO_WritePin(GPIOD , GPIO_PIN_5 , GPIO_PIN_RESET); //485/232_3
				break;

				case RS422:
					HAL_GPIO_WritePin(GPIOD , GPIO_PIN_5 , GPIO_PIN_SET);		//485/232模式
					HAL_GPIO_WritePin(GPIOD , GPIO_PIN_2 , GPIO_PIN_RESET);//全双工
				break;

				case RS485:
					HAL_GPIO_WritePin(GPIOD , GPIO_PIN_5 , GPIO_PIN_SET);		//485/422模式
					HAL_GPIO_WritePin(GPIOD , GPIO_PIN_2 , GPIO_PIN_RESET);//全双工
				break;		
			}
		break;
		
		case U7:
			GPIO_InitStruct.Pin   = GPIO_PIN_10;
			GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
			GPIO_InitStruct.Pull  = GPIO_PULLUP;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
			HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);			

			GPIO_InitStruct.Pin   = GPIO_PIN_14 | GPIO_PIN_15 | GPIO_PIN_10;
			GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
			GPIO_InitStruct.Pull  = GPIO_PULLUP;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
			HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);		

			GPIO_InitStruct.Pin   = GPIO_PIN_14 | GPIO_PIN_13;
			GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
			GPIO_InitStruct.Pull  = GPIO_PULLUP;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
			HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
		
			HAL_GPIO_WritePin(GPIOE , GPIO_PIN_14 , GPIO_PIN_SET); //SHDN 	低电平时启动低功耗
			HAL_GPIO_WritePin(GPIOE , GPIO_PIN_13 , GPIO_PIN_SET); //SLEW		低电平时限制速度250K
			HAL_GPIO_WritePin(GPIOB , GPIO_PIN_14 , GPIO_PIN_RESET);//485_EN	低电平时启动485接收
			HAL_GPIO_WritePin(GPIOB , GPIO_PIN_10 , GPIO_PIN_SET);  //T2IN, DE 高电平时启动驱动程序		
			switch(sm)
			{
				case RS232:
					HAL_GPIO_WritePin(GPIOB , GPIO_PIN_15 , GPIO_PIN_RESET); //485/232_3
				break;

				case RS422:
					HAL_GPIO_WritePin(GPIOB,  GPIO_PIN_15 , GPIO_PIN_SET);		//485/232模式
					HAL_GPIO_WritePin(GPIOD , GPIO_PIN_10 , GPIO_PIN_RESET);//全双工
				break;

				case RS485:
					HAL_GPIO_WritePin(GPIOB , GPIO_PIN_15 , GPIO_PIN_SET);		//485/422模式
					HAL_GPIO_WritePin(GPIOD , GPIO_PIN_10 , GPIO_PIN_RESET);	//全双工
				break;		
			}
			break;
		
		case U8:
			GPIO_InitStruct.Pin   = GPIO_PIN_8;
			GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
			GPIO_InitStruct.Pull  = GPIO_PULLUP;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
			HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

			GPIO_InitStruct.Pin   = GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
			GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
			GPIO_InitStruct.Pull  = GPIO_PULLUP;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
			HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);		
		
			HAL_GPIO_WritePin(GPIOA , GPIO_PIN_8 , GPIO_PIN_SET); //SHDN 	低电平时启动低功耗
			HAL_GPIO_WritePin(GPIOD , GPIO_PIN_15 , GPIO_PIN_SET); //SLEW		低电平时限制速度250K
			HAL_GPIO_WritePin(GPIOD , GPIO_PIN_12 , GPIO_PIN_RESET);//485_EN	低电平时启动485接收
			HAL_GPIO_WritePin(GPIOD , GPIO_PIN_11 , GPIO_PIN_SET);//T2IN, DE 高电平时启动驱动程序
			switch(sm)
			{
				case RS232:
					HAL_GPIO_WritePin(GPIOD , GPIO_PIN_14 , GPIO_PIN_RESET); //485/232_2
				break;

				case RS422:
					HAL_GPIO_WritePin(GPIOD , GPIO_PIN_14 , GPIO_PIN_SET);		//485/422模式
					HAL_GPIO_WritePin(GPIOD , GPIO_PIN_13 , GPIO_PIN_RESET);//全双工
				break;

				case RS485:
					HAL_GPIO_WritePin(GPIOD , GPIO_PIN_14 , GPIO_PIN_SET);		//485/422模式
					HAL_GPIO_WritePin(GPIOD , GPIO_PIN_13 , GPIO_PIN_RESET);//全双工
				break;		
			}
			break;	

		case U26:
			GPIO_InitStruct.Pin   = GPIO_PIN_8;
			GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
			GPIO_InitStruct.Pull  = GPIO_PULLUP;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
			HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);		
		
			GPIO_InitStruct.Pin   = GPIO_PIN_6 | GPIO_PIN_7;
			GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
			GPIO_InitStruct.Pull  = GPIO_PULLUP;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
			HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);		
		
			GPIO_InitStruct.Pin   = GPIO_PIN_1 | GPIO_PIN_0 | GPIO_PIN_9;
			GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
			GPIO_InitStruct.Pull  = GPIO_PULLUP;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
			HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);	
		
			HAL_GPIO_WritePin(GPIOE , GPIO_PIN_1 , GPIO_PIN_SET); 	//SHDN 	低电平时启动低功耗
			HAL_GPIO_WritePin(GPIOE , GPIO_PIN_0 , GPIO_PIN_SET); 	//SLEW		低电平时限制速度250K
			HAL_GPIO_WritePin(GPIOD , GPIO_PIN_7 , GPIO_PIN_RESET);	//485_EN	低电平时启动485接收
			HAL_GPIO_WritePin(GPIOD , GPIO_PIN_6 , GPIO_PIN_SET);		//T2IN, DE 高电平时启动驱动程序 422_3
			switch(sm)
			{
				case RS232:
					HAL_GPIO_WritePin(GPIOD , GPIO_PIN_5 , GPIO_PIN_RESET); 
				break;

				case RS422:
					HAL_GPIO_WritePin(GPIOD , GPIO_PIN_5 , GPIO_PIN_SET); 
					HAL_GPIO_WritePin(GPIOB , GPIO_PIN_8 , GPIO_PIN_RESET);
				break;

				case RS485:
					HAL_GPIO_WritePin(GPIOD , GPIO_PIN_5 , GPIO_PIN_SET); 
					HAL_GPIO_WritePin(GPIOB , GPIO_PIN_8 , GPIO_PIN_RESET);
				break;	

				default:
					break;
			}
			break;	

			default:
			break;			
	}
}	
	
	
/**
  * @brief
  * @param
  * @retval
  */
void Get_GPIO(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;

	/* 打开GPIO时钟 */
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	/* 配置GPIO */
	GPIO_InitStruct.Pin = GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;   		/* 设置推挽输出 */
	GPIO_InitStruct.Pull = GPIO_NOPULL;                 /* 上下拉电阻不使能 */
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  		/* GPIO速度等级 */
	HAL_GPIO_Init(GPIOD , &GPIO_InitStruct);


	GPIO_InitStruct.Pin =  GPIO_PIN_3|GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;   		/* 设置推挽输出 */
	GPIO_InitStruct.Pull = GPIO_NOPULL;                 /* 上下拉电阻不使能 */
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  		/* GPIO速度等级 */
	HAL_GPIO_Init(GPIOB , &GPIO_InitStruct);
}	


/**
  * @brief
  * @param
  * @retval
  */
void bsp_InitGpio(void)
{
	/*
	配置所有的LED指示灯GPIO为推挽输出模式
	由于将GPIO设置为输出时，GPIO输出寄存器的值缺省是0，因此会驱动LED点亮.
	这是我不希望的，因此在改变GPIO为输出前，先关闭LED指示灯
	*/
//	bsp_GpioOff(1);
//	bsp_GpioOff(2);
//	bsp_GpioOff(3);
//	bsp_GpioOff(4);
//	bsp_GpioOff(5);
//	bsp_GpioOff(6);
//	bsp_GpioOff(7);
	
}

/**
  * @brief
  * @param
  * @retval
  */
void GPIO_LEDInitialization(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	GPIO_InitStructure.Pin 		= GPIO_PIN_2|GPIO_PIN_3;
	GPIO_InitStructure.Mode 	= GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull 	= GPIO_PULLUP;
	GPIO_InitStructure.Speed 	= GPIO_SPEED_FREQ_HIGH;
	
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
}




