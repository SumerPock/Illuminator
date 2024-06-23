
#ifndef __BSP_H
#define __BSP_H

#include "EventRecorder.h"
#include "EventRecorderConf.h"
#include <stdio.h>
#include <string.h>
#include "includes.h"
#include "bsp_dwt.h"
#include "stm32f4xx_hal.h"

#include "main.h"
#include "PUDI_APP.h"  
#include "stdlib.h"
#include "stdio.h"

#include "lwip/udp.h"
#include "bsp_can.h"
#include "bsp_gpio.h"
#include "bsp_uart_fifo.h"
#include "bsp_dma.h"
#include "bsp_uart_dma.h"

#include "ring_buffer.h"
#include "ring_buffer_chapter.h"

#include "udp_echoserver.h"

#include "Task_can.h"
#include "Task_led.h"
#include "Task_lwip.h"
#include "Task_steer_gear.h"

#include "SEGGER_RTT.h"

//#include "Task_start.h"

/* 开关全局中断的宏 */
#define ENABLE_INT()	__set_PRIMASK(0)	/* 使能全局中断 */
#define DISABLE_INT()	__set_PRIMASK(1)	/* 禁止全局中断 */


//#define	DEBUGPR 1


extern int16_t Data_out[200];

extern unsigned short Cal_Sum(uint8_t * Data_In , uint8_t len);
extern void out_sine(unsigned char gethz, unsigned char getdu, unsigned short* getnumber, short dataout[200]);
//extern void out_sine(unsigned char gethz , unsigned char getdu);
/*环形队列创建*/
extern unsigned char adcbuff[4096]; 
extern ring_buffer rb_adc_rx;

/*环形队列*/
//extern unsigned char spibuff[4096]; 
//extern ring_buffer rb_spi_rx;	

/*舵机出厂检测1模式*/
extern unsigned char steering_gear_mode1[4096]; 
extern ring_buffer stgear_rx;	


/*舵机出厂检测2模式*/
extern unsigned char steering_gear_mode2[4096]; 
extern ring_buffer stgear_rxmode2;							/*环形队列*/

/* 保存定时器ID */
extern void timer_Periodic_App(void *argument);
extern osTimerId_t  timerID_Periodic;        				
extern const osTimerAttr_t timer_Periodic_Attr;


extern void SystemClock_Config(void);
extern uint8_t PUDI_ReadAngle(uint8_t *state,uint8_t *CRC8, int32_t* ReadRegData);
extern void bsp_Spi_Cs_InitGpio(void);
extern void bsp_LED_InitGpio(void);

extern ADC_HandleTypeDef hadc1;


/*udp通用接收函数*/
struct Get_UdPData
{
	unsigned char ucudpdata[50];
};

//#pragma pack(2)

/*上位机下发舵机板控制协议*/
struct GETUDPDATA
{
	unsigned char ucGetUdpHand[2];	//帧头
	unsigned char ucGetUdploop;		//帧序号
	unsigned char ucGetUdpID;			//ID
	unsigned char ucSetUdpSampling_Rate[2];	  //采样率
	unsigned char ucSetUdpSampling_Number[2]; //采样数量	
	
	/*具体数据*/
	unsigned char dataucGetUdpHand[2]; 	//具体数据帧头
	unsigned char dataucGetUdplen;			//帧长
	unsigned char dataucGetUdploop[2];	//流水号
	
	unsigned char dataucsteering_gear1[2];	//舵机指令1L
	unsigned char dataucsteering_gear2[2];	//舵机指令2L
	
	unsigned char dataucChick[2];
	unsigned char dataucend;
};





/*舵机出厂检测1.下位机发送给舵机驱动板的数据*/
struct STEER_UART_DATA
{
	unsigned short 	steer_Hand;			//帧头
	unsigned char 	steer_len;			//帧长
	unsigned short  steer_loop;			//帧序号
	unsigned short 	steering_gear1;		//舵机指令1
	unsigned short  steering_gear2;		//舵机指令2
	unsigned short  chickdata;				//校验码
	unsigned char  enddata;					//帧尾
};

/*舵机出厂检测1.下位机向上给上位机报的值*/
struct COM_Steering_gear
{
	unsigned char ucSetUdpHand[2];			//头
	unsigned char ucSetlen;							//长度
	unsigned char ucSetUdploop[2];						//序号
	unsigned char ucSetUdpID;									//ID
	unsigned char ucSetUdpSampling_Rate[2];	  //采样率
	unsigned char ucSetUdpSampling_Number[2]; //采样数量
	unsigned char getucSetlen[2];				//接收长度
	unsigned char steering_gear1[2];	//舵机1
	unsigned char steering_gear2[2];	//舵机2
	unsigned char spi_data[2];				//SPI
	unsigned char chick[2];						//校验
};

/*下位机收到 -> 舵机参数修改*/
struct Parameter_Steering_gear
{
	unsigned char ucpsdata[25];
};

/*下位机下发 -> 舵机参数修改*/



/*电位计入场验收. UDP数据打包发送*/
struct SETUDPDATA
{
	unsigned char ucSetUdpHand[2];						//头
	unsigned char ucSetUdploop;								//序号
	unsigned char ucSetUdpID;									//ID
	unsigned char ucSetUdpSampling_Rate[2];	  //采样率
	unsigned char ucSetUdpSampling_Number[2]; //采样数量
	unsigned char getdata[800];								//震动ADC数据 200包
};


/*舵机出厂检测1*/
struct Steering_gear_Mode1
{
	unsigned char SgUdpHand[2];		//头
	unsigned char SgUdpNum;				//帧序号
	unsigned char SgUdpID;				//帧ID
	unsigned char SgUdpSampling_Rate[2];		//采样率
	unsigned char SgUdpSampling_Number[2];	//采样数
	unsigned char SgUdpSteering_gear_spi[800];
};


typedef struct
{
	unsigned char *pRxBuf;
	unsigned char rxFlag;
	unsigned char rxLen;
	const unsigned char rxSize;	/*const 必须在结构体变量定义时赋值 */
}BspUart_t;

extern BspUart_t g_tBspUsart1;
extern BspUart_t g_tBspUart4;
extern BspUart_t g_tBspUart5;
extern BspUart_t g_tBspUsart2;
extern BspUart_t g_tBspUsart3;
extern BspUart_t g_tBspUsart6;

extern struct GETUDPDATA SetUdpData;




void Error_Handler(void);
/**********	RTX5引用	***********/

/************************* 消息队列 *******************************/

/*用于接收网络UDP数据*/
extern osMessageQueueId_t msgQueue_UdpResData; 			
extern const osMessageQueueAttr_t UdpResData_Attr;


/*串口1缓存数据*/		
extern osMessageQueueId_t msgQueue_rxuart1Data; 	
extern const osMessageQueueAttr_t rxuart1Data_Attr;


/************************* 事件标志组 *******************************/

/*网线插拔标志 事件标志组*/
extern osEventFlagsId_t event_cable_ID;
extern const osEventFlagsAttr_t event_cable_Attr;

/* 网口获取数据接 事件标志组属性 */
extern const osEventFlagsAttr_t event_UdpGetData_Attr;
extern osEventFlagsId_t event_UdpGetData_ID;

/* 采样控制 */
extern const osEventFlagsAttr_t event_Sampling_Attr;
extern osEventFlagsId_t event_Sampling_ID;


extern const osEventFlagsAttr_t event_HzMode_Attr;
extern osEventFlagsId_t event_HzMode_ID;
/************************* 任务 *******************************/

extern void AppTaskGetDataAndUDP(void *argument);	/*UDP网络打包任务*/
extern const osThreadAttr_t ThreadGetDataAndUDP_Attr;
extern osThreadId_t ThreadIdTaskGetDataAndUDP;


extern osThreadId_t ThreadIdStart;								/*启动任务*/
extern void AppTaskUDPSend(void *argument);
extern const osThreadAttr_t TaskUDPSend_Attr;
extern osThreadId_t ThreadIdTaskUDPSend;


extern osThreadId_t ThreadIdTaskLwIPPro;					/*LWIP网络轮询*/
extern const osThreadAttr_t ThreadLwIPPro_Attr;
extern void AppTaskLwIPPro(void *argument);

extern osThreadId_t ThreadIdTaskEthCheck;					/*网线插拔*/
extern void AppTaskEthCheck(void *argument);
extern const osThreadAttr_t ThreadEthCheck_Attr;


extern void AppTaskUdpNetWork(void *argument);	/* UDP网络任务解析 */
extern const osThreadAttr_t ThreadUdpNetWork_Attr;
extern osThreadId_t ThreadIdTaskUdpNetWork;


extern void AppTaskSteering_gear(void *argument);	/*舵机指令解析任务*/
extern const osThreadAttr_t ThreadSteering_gear_Attr;
extern osThreadId_t ThreadIdTaskSteering_gear;

extern void AppTaskARMSteer_gear(void *argument);
extern const osThreadAttr_t ThreadARMSteer_gear_Attr;
extern osThreadId_t ThreadIdTaskARMSteer_gear;


#endif


