
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
#include "bsp_dwt.h"

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






// 定义结构体来存储属性
typedef struct
{
    unsigned int arrset;       	// ARR设定值
    unsigned int arrtimelow;   	// ARR设定低电平时间
    unsigned int arrtimehight; 	// ARR设定高电平时间
    int pulseCount;   					// 脉冲次数
		int pulseCountSet;					//设置的脉冲次数
    unsigned int nflagGpio;    	// GpioToogleSet函数内部状态机
    unsigned int nflagtime2;   	// 定时器2触发标志
} GpioToogleSetPar;
extern GpioToogleSetPar setpar;
//#define	DEBUGPR 1


struct preciseCoding
{
	short 					Frequency; //次数
	unsigned short  CodCycle;	 //周期
};







/*udp通用接收函数*/
struct Get_UdPData
{
	unsigned char nUdpD_size;
	unsigned char arrUdp_index[128];
};


///*下位机收到 -> 舵机参数修改*/
//struct Parameter_Steering_gear
//{
//	unsigned char ucpsdata[25];
//};


//struct baseCoding
//{
//	signed short 		baseCodFrequency; //次数
//	unsigned short  baseCodCycle;			//周期
//	
//	unsigned short  baseCFVerify; 		//次数验证
//	unsigned short  baseCCVerify;			//周期验证	
//};


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


/***引用部分***/

extern void Update_Timer_Arr(uint32_t new_arr_value);

extern TIM_HandleTypeDef htim2;
extern void SystemClock_Config(void);
extern void MX_TIM2_Init(void);
extern void GpioToogleSet(GpioToogleSetPar *rect);
extern void Error_Handler(void);
/**********	RTX5引用	***********/

/************************* 消息队列 *******************************/

/*用于接收网络UDP数据*/
extern osMessageQueueId_t msgQueue_UdpResData; 			
extern const osMessageQueueAttr_t UdpResData_Attr;


/*串口1缓存数据*/		
extern osMessageQueueId_t msgQueue_rxuart1Data; 	
extern const osMessageQueueAttr_t rxuart1Data_Attr;

/* 保存定时器ID */
extern void timer_Periodic_App(void *argument);
extern osTimerId_t  timerID_Periodic;        				
extern const osTimerAttr_t timer_Periodic_Attr;

/*传输精确频率码信息*/
extern osMessageQueueId_t msgQueue_PreFreData; 
extern const osMessageQueueAttr_t PreFreData_Attr;

/************************* 事件标志组 *******************************/

/*网线插拔标志 事件标志组*/
extern osEventFlagsId_t event_cable_ID;
extern const osEventFlagsAttr_t event_cable_Attr;

///* 网口获取数据接 事件标志组属性 */
//extern const osEventFlagsAttr_t event_UdpGetData_Attr;
//extern osEventFlagsId_t event_UdpGetData_ID;

/* 采样控制 */
extern const osEventFlagsAttr_t event_Sampling_Attr;
extern osEventFlagsId_t event_Sampling_ID;


extern const osEventFlagsAttr_t event_HzMode_Attr;
extern osEventFlagsId_t event_HzMode_ID;

/*PWM任务控制*/
extern const osEventFlagsAttr_t event_PWMTaskFlag_Attr;
extern osEventFlagsId_t event_PWMTaskFlag_ID;

extern const osEventFlagsAttr_t event_Time2Flag_Attr;
extern osEventFlagsId_t event_Time2Flag_ID;

/************************* 任务 *******************************/


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

extern void AppTaskSetPWM(void *argument);
extern const osThreadAttr_t ThreadSetPWM_Attr;
extern osThreadId_t ThreadIdTaskSetPWM;
#endif


