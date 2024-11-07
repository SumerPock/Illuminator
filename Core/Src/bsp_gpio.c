#include "bsp.h"

/* ���ƶ˿ڶ�Ӧ��RCCʱ�� */
#define GPIO_GPIO_CLK_ALLENABLE() {	  \
		__HAL_RCC_GPIOD_CLK_ENABLE(); \
		__HAL_RCC_GPIOC_CLK_ENABLE(); \
		__HAL_RCC_GPIOA_CLK_ENABLE(); \
	};
/*����GPIO�ص����ƺ���*/
//1 . �������������ֱ�ʵ��GPIO�������͹���


/**
  * @brief
  * @param
  * @retval
  */
void Get_GPIO(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;

	/* ��GPIOʱ�� */
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	/* ����GPIO */
	GPIO_InitStruct.Pin = GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;   		/* ����������� */
	GPIO_InitStruct.Pull = GPIO_NOPULL;                 /* ���������費ʹ�� */
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  		/* GPIO�ٶȵȼ� */
	HAL_GPIO_Init(GPIOD , &GPIO_InitStruct);


	GPIO_InitStruct.Pin =  GPIO_PIN_3|GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;   		/* ����������� */
	GPIO_InitStruct.Pull = GPIO_NOPULL;                 /* ���������費ʹ�� */
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  		/* GPIO�ٶȵȼ� */
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
	�������е�LEDָʾ��GPIOΪ�������ģʽ
	���ڽ�GPIO����Ϊ���ʱ��GPIO����Ĵ�����ֵȱʡ��0����˻�����LED����.
	�����Ҳ�ϣ���ģ�����ڸı�GPIOΪ���ǰ���ȹر�LEDָʾ��
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




