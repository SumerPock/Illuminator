
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

/* ����ȫ���жϵĺ� */
#define ENABLE_INT()	__set_PRIMASK(0)	/* ʹ��ȫ���ж� */
#define DISABLE_INT()	__set_PRIMASK(1)	/* ��ֹȫ���ж� */






// ����ṹ�����洢����
typedef struct
{
    unsigned int arrset;       	// ARR�趨ֵ
    unsigned int arrtimelow;   	// ARR�趨�͵�ƽʱ��
    unsigned int arrtimehight; 	// ARR�趨�ߵ�ƽʱ��
    int pulseCount;   					// �������
		int pulseCountSet;					//���õ��������
    unsigned int nflagGpio;    	// GpioToogleSet�����ڲ�״̬��
    unsigned int nflagtime2;   	// ��ʱ��2������־
} GpioToogleSetPar;
extern GpioToogleSetPar setpar;
//#define	DEBUGPR 1


struct preciseCoding
{
	short 					Frequency; //����
	unsigned short  CodCycle;	 //����
};







/*udpͨ�ý��պ���*/
struct Get_UdPData
{
	unsigned char nUdpD_size;
	unsigned char arrUdp_index[128];
};


///*��λ���յ� -> ��������޸�*/
//struct Parameter_Steering_gear
//{
//	unsigned char ucpsdata[25];
//};


//struct baseCoding
//{
//	signed short 		baseCodFrequency; //����
//	unsigned short  baseCodCycle;			//����
//	
//	unsigned short  baseCFVerify; 		//������֤
//	unsigned short  baseCCVerify;			//������֤	
//};


typedef struct
{
	unsigned char *pRxBuf;
	unsigned char rxFlag;
	unsigned char rxLen;
	const unsigned char rxSize;	/*const �����ڽṹ���������ʱ��ֵ */
}BspUart_t;

extern BspUart_t g_tBspUsart1;
extern BspUart_t g_tBspUart4;
extern BspUart_t g_tBspUart5;
extern BspUart_t g_tBspUsart2;
extern BspUart_t g_tBspUsart3;
extern BspUart_t g_tBspUsart6;

extern struct GETUDPDATA SetUdpData;


/***���ò���***/

extern void Update_Timer_Arr(uint32_t new_arr_value);

extern TIM_HandleTypeDef htim2;
extern void SystemClock_Config(void);
extern void MX_TIM2_Init(void);
extern void GpioToogleSet(GpioToogleSetPar *rect);
extern void Error_Handler(void);
/**********	RTX5����	***********/

/************************* ��Ϣ���� *******************************/

/*���ڽ�������UDP����*/
extern osMessageQueueId_t msgQueue_UdpResData; 			
extern const osMessageQueueAttr_t UdpResData_Attr;


/*����1��������*/		
extern osMessageQueueId_t msgQueue_rxuart1Data; 	
extern const osMessageQueueAttr_t rxuart1Data_Attr;

/* ���涨ʱ��ID */
extern void timer_Periodic_App(void *argument);
extern osTimerId_t  timerID_Periodic;        				
extern const osTimerAttr_t timer_Periodic_Attr;

/*���侫ȷƵ������Ϣ*/
extern osMessageQueueId_t msgQueue_PreFreData; 
extern const osMessageQueueAttr_t PreFreData_Attr;

/************************* �¼���־�� *******************************/

/*���߲�α�־ �¼���־��*/
extern osEventFlagsId_t event_cable_ID;
extern const osEventFlagsAttr_t event_cable_Attr;

///* ���ڻ�ȡ���ݽ� �¼���־������ */
//extern const osEventFlagsAttr_t event_UdpGetData_Attr;
//extern osEventFlagsId_t event_UdpGetData_ID;

/* �������� */
extern const osEventFlagsAttr_t event_Sampling_Attr;
extern osEventFlagsId_t event_Sampling_ID;


extern const osEventFlagsAttr_t event_HzMode_Attr;
extern osEventFlagsId_t event_HzMode_ID;

/*PWM�������*/
extern const osEventFlagsAttr_t event_PWMTaskFlag_Attr;
extern osEventFlagsId_t event_PWMTaskFlag_ID;

extern const osEventFlagsAttr_t event_Time2Flag_Attr;
extern osEventFlagsId_t event_Time2Flag_ID;

/************************* ���� *******************************/


extern osThreadId_t ThreadIdStart;								/*��������*/
extern void AppTaskUDPSend(void *argument);
extern const osThreadAttr_t TaskUDPSend_Attr;
extern osThreadId_t ThreadIdTaskUDPSend;


extern osThreadId_t ThreadIdTaskLwIPPro;					/*LWIP������ѯ*/
extern const osThreadAttr_t ThreadLwIPPro_Attr;
extern void AppTaskLwIPPro(void *argument);

extern osThreadId_t ThreadIdTaskEthCheck;					/*���߲��*/
extern void AppTaskEthCheck(void *argument);
extern const osThreadAttr_t ThreadEthCheck_Attr;


extern void AppTaskUdpNetWork(void *argument);	/* UDP����������� */
extern const osThreadAttr_t ThreadUdpNetWork_Attr;
extern osThreadId_t ThreadIdTaskUdpNetWork;


extern void AppTaskSteering_gear(void *argument);	/*���ָ���������*/
extern const osThreadAttr_t ThreadSteering_gear_Attr;
extern osThreadId_t ThreadIdTaskSteering_gear;

extern void AppTaskARMSteer_gear(void *argument);
extern const osThreadAttr_t ThreadARMSteer_gear_Attr;
extern osThreadId_t ThreadIdTaskARMSteer_gear;

extern void AppTaskSetPWM(void *argument);
extern const osThreadAttr_t ThreadSetPWM_Attr;
extern osThreadId_t ThreadIdTaskSetPWM;
#endif


