#ifndef __BSP_GPIO_H
#define __BSP_GPIO_H

/*宏定义是函数的临时演员，其会在编译后被替换调，所以不会过函数编译检查
，不能以函数的眼光看待宏定义！！！*/
#define LED_NUMBER_2	GPIOE,GPIO_PIN_10
#define LED_NUMBER_1	GPIOE,GPIO_PIN_11

#define LIGHT GPIO_PIN_SET
#define CLOSE GPIO_PIN_RESET
//enum control{
//	light = 0,
//	close = 1
//};

#define LED_CONTROL(a , b)	HAL_GPIO_WritePin(a , b)
 
typedef enum             /* Defines an enumeration type    */
{
	RS232 = 0,
	RS422 = 1,
	RS485 = 2
}SP330MODE;


typedef enum             /* Defines an enumeration type    */
{
	U1 = 0,
	U7 = 1,
	U8 = 2,
	U26 = 3
}SP330NUM; 
 
extern void ConfigSP330(unsigned char num , unsigned char mode);

extern void bsp_InitGpio(void);

extern void bsp_GpioOn(unsigned char _no);

extern void bsp_GpioOff(unsigned char _no);

extern void bsp_GpioToggle(unsigned char _no);

extern unsigned char bsp_IsGpioOn(unsigned char _no);

void GPIO_LEDInitialization(void);

void GPIO_OUTPUT(void);

void Get_GPIO(void);

#endif