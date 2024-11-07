/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
#include "includes.h"
#include "bsp.h"
#include <math.h>


/************************************** 全局变量 *******************************************/

///*电位计入场验收*/
//unsigned char adcbuff[4096]; 
//ring_buffer rb_adc_rx;							/*环形队列*/
///*舵机出厂检测1模式*/
//unsigned char steering_gear_mode1[4096]; 
//ring_buffer stgear_rx;							/*环形队列*/
///*舵机出厂检测2模式*/
//unsigned char steering_gear_mode2[4096]; 
//ring_buffer stgear_rxmode2;					/*环形队列*/

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim6;


/* Private function prototypes -----------------------------------------------*/
static void MX_GPIO_Init(void);


/************************************** 任务 *******************************************/

/*	
AppTaskEthCheck
1.	任务名：		
		网线插拔检测
2.	任务工作内容：		
		检测网线是否插入，插入后进行网络链接,然后此退出任务
3.	优先级：		 
		High1
4.	任务类型
		250ms轮询执行
*/
void AppTaskEthCheck(void *argument);
const osThreadAttr_t ThreadEthCheck_Attr =
{
	.name = "osRtxEthCheckThread",
	.attr_bits = osThreadDetached, 
	.priority = osPriorityHigh1,
	.stack_size = 1024,
};
osThreadId_t ThreadIdTaskEthCheck = NULL;	

/*	
AppTaskLwIPPro
1.	任务名：		
		LWIP网络数据接收轮询
2.	任务工作：	
		轮询接收网络数据
3.	优先级：		
		High
4.	任务类型
		20ms轮询执行
*/
void AppTaskLwIPPro(void *argument);
const osThreadAttr_t ThreadLwIPPro_Attr = 		
{
	.name = "osRtxLwIPProThread",
	.attr_bits = osThreadDetached, 
	.priority = osPriorityHigh,
	.stack_size = 2048,
};
osThreadId_t ThreadIdTaskLwIPPro = NULL;	



/*
AppTaskUdpNetWork
1.	任务名：	
2.	任务工作：	
		1.	接收上位机舵机角度控制指令，并通过DMA发送
		2.	接收上位机的正选运动模式，并持续给舵机发送正弦运动指令
		3.	接收上位机的停止采样模式
		4.	接收上位机的电位计入场验收模式，
		5.	接收上位机的舵机参数纠正模式，通过DMA发送
3.	优先级：
		High6
4.	任务类型
		堵塞性任务

*/
void AppTaskUdpNetWork(void *argument);					
const osThreadAttr_t ThreadUdpNetWork_Attr =
{
	.name = "osRtxUdpNetWorkThread",
	.attr_bits = osThreadDetached, 
	.priority = osPriorityHigh6,
	.stack_size = 4096,
};
osThreadId_t ThreadIdTaskUdpNetWork = NULL;




/**
AppTaskStart
1.	任务名：
			启动任务
2.	任务工作：
3.	优先级：
			High7
6.	任务类型
			执行过后关闭
**/
void AppTaskStart(void *argument);				
const osThreadAttr_t ThreadStart_Attr =
{
	.name = "osRtxStartThread",
	.attr_bits = osThreadDetached, 
	.priority = osPriorityHigh7,
	.stack_size = 2048,
};
osThreadId_t ThreadIdStart = NULL;


/**
AppTaskARMSteer_gear
1.	任务名：
2.	任务工作：
			下发正弦运动模式数据，并将当前发送的正弦角度和接收到的舵机反馈角度存入队列中
3.	优先级：
			High5
4.	任务类型
			堵塞性任务
**/
void AppTaskARMSteer_gear(void *argument);		
const osThreadAttr_t ThreadARMSteer_gear_Attr = 
{
	.name = "osRtxARMSteer_gearThread",
	.attr_bits = osThreadDetached, 
	.priority = osPriorityHigh5,
	.stack_size = 4096,
};
osThreadId_t ThreadIdTaskARMSteer_gear = NULL;


/**
AppTaskSetPWM
1.	任务名：
2.	任务工作：
			1-200Hz脉冲频率
3.	优先级：
			High7
4.	任务类型
**/
void AppTaskSetPWM(void *argument);
const osThreadAttr_t ThreadSetPWM_Attr =
{
	.name = "osRtxSetPWMThread",
	.attr_bits = osThreadDetached, 
	.priority = osPriorityISR,
	.stack_size = 2048,
};
osThreadId_t ThreadIdTaskSetPWM = NULL;

/************************************** 定时器 *******************************************/
void timer_Periodic_App(void *argument);
osTimerId_t  timerID_Periodic = NULL;
const osTimerAttr_t timer_Periodic_Attr = { .name = "periodic timer", };

/************************************** 消息队列 *******************************************/
osMessageQueueId_t msgQueue_UdpResData;				/*接收UDP数据*/
const osMessageQueueAttr_t UdpResData_Attr ={.name = "UdpResData",};

osMessageQueueId_t msgQueue_rxuart1Data; 			/*串口1缓存数据*/		
const osMessageQueueAttr_t rxuart1Data_Attr ={.name = "rxuart1Data", };

osMessageQueueId_t msgQueue_PreFreData; /*传输精确频率码信息*/
const osMessageQueueAttr_t PreFreData_Attr ={.name = "PreFreData", };

/************************************** 事件标志位 **********************************************/

/* 网线插拔事件标志组属性 */
const osEventFlagsAttr_t event_cable_Attr = { .name = "event_cable", };
osEventFlagsId_t event_cable_ID = NULL;

/*PWM任务控制*/
const osEventFlagsAttr_t event_PWMTaskFlag_Attr = {.name = "event_PwmTaskFlag"};
osEventFlagsId_t event_PWMTaskFlag_ID = NULL;

const osEventFlagsAttr_t event_Time2Flag_Attr = {.name = "event_Time2Flag",};
osEventFlagsId_t event_Time2Flag_ID = NULL;

GpioToogleSetPar setpar = {839999, 2519999, 0, 0, 0, 0};

void gpiodemo(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	__HAL_RCC_GPIOA_CLK_ENABLE();	
	__HAL_RCC_GPIOE_CLK_ENABLE();
	GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
		GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
}

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
	/* MCU Configuration--------------------------------------------------------*/
	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();
	/* Configure the system clock */
	SystemClock_Config();
	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	HAL_SuspendTick();
	osKernelInitialize();                                  
	ThreadIdStart = osThreadNew(AppTaskStart, NULL, &ThreadStart_Attr);  
	osKernelStart();
}

/**
  * @brief  启动任务
  * @retval None
  */
void AppTaskStart(void *argument)
{
	const uint16_t usFrequency = 100;
	uint32_t tick;	
	HAL_ResumeTick();
	EventRecorderInitialize(EventRecordAll, 1U);						/* 初始化EventRecorder并开启 */
	EventRecorderStart();
	MX_TIM2_Init();	
//	if(HAL_TIM_Base_Start_IT(&htim2) != HAL_OK){
//	}	
//	setpar.nflagGpio = 1;
	gpiodemo();
	
//	SEGGER_RTT_ConfigUpBuffer(0, "RTTUP", NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);			/* 配置通道0，上行配置*/
//	SEGGER_RTT_ConfigDownBuffer(0, "RTTDOWN", NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);	/* 配置通道0，下行配置*/	
//	SEGGER_RTT_SetTerminal(0);
//	SEGGER_RTT_printf(0, "the project is runing \r\n");
		
	msgQueue_UdpResData  = osMessageQueueNew(12 , sizeof(struct Get_UdPData) , &UdpResData_Attr); /* 接收UDP数据消息队列*/	
	msgQueue_PreFreData  = osMessageQueueNew(12 , sizeof(struct preciseCoding) , &PreFreData_Attr);	
	if(net_init() != 0){
		//printf("lwip is config fild \r\n");
	}
	
	/* LWIP网络 udp轮询任务 */
	ThreadIdTaskLwIPPro = osThreadNew(AppTaskLwIPPro, NULL, &ThreadLwIPPro_Attr);	
	tick += usFrequency + 500;                          
	osDelayUntil(tick);	
	
	/* 网线插拔状态检测*/	
	ThreadIdTaskEthCheck = osThreadNew(AppTaskEthCheck, NULL, &ThreadEthCheck_Attr); 	
	tick += usFrequency + 500;                          
	osDelayUntil(tick);		

	/* UDP 网络数据解析任务*/
	ThreadIdTaskUdpNetWork = osThreadNew(AppTaskUdpNetWork, NULL, &ThreadUdpNetWork_Attr);	
	tick += usFrequency + 500;                          
	osDelayUntil(tick);
	
	event_PWMTaskFlag_ID = osEventFlagsNew(&event_PWMTaskFlag_Attr);	
	/* 这个定时器浪费了很多资源在需要关闭的时候关闭它 */

	tick = osKernelGetTickCount(); 
	while(1){
		tick += usFrequency;                          
		osDelayUntil(tick);
		osThreadExit();
	}
}




static unsigned char LoopState = 0;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim == (&htim2))
	{
		switch(setpar.nflagGpio)
		{
			case 1:
				if(setpar.pulseCountSet > 0)
				{
					setpar.pulseCount++;
					if(setpar.pulseCount < setpar.pulseCountSet)
					{
						HAL_GPIO_WritePin(GPIOE , GPIO_PIN_10 , GPIO_PIN_SET);
						bsp_DelayUS(5);	
						HAL_GPIO_WritePin(GPIOE , GPIO_PIN_10 , GPIO_PIN_RESET);
					}
					else 
					{
						if(HAL_TIM_Base_Stop_IT(&htim2)!= HAL_OK){
						}						
					}
				}else{
					//无限次
						HAL_GPIO_WritePin(GPIOE , GPIO_PIN_10 , GPIO_PIN_SET);
						bsp_DelayUS(5);	
						HAL_GPIO_WritePin(GPIOE , GPIO_PIN_10 , GPIO_PIN_RESET);				
				}

				//setpar.nflagGpio = 2;
				break;
		}
		
		
		
		osEventFlagsSet(event_Time2Flag_ID , 0x01U << 0);	
//		GpioToogleSet(&setpar);
//		if (LoopState == 0)
//		{
//			//HAL_GPIO_WritePin(GPIOE , GPIO_PIN_11 , GPIO_PIN_RESET);
//			//HAL_GPIO_WritePin(GPIOE , GPIO_PIN_10 , GPIO_PIN_SET);
//			HAL_GPIO_WritePin(GPIOE , GPIO_PIN_10 , GPIO_PIN_SET);
//			bsp_DelayUS(5);			
//			HAL_GPIO_WritePin(GPIOE , GPIO_PIN_10 , GPIO_PIN_RESET);
//			

//			
//		}			
//		LoopState = !LoopState;			
	}
}


/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void){
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
}

