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

static void MX_TIM6_Init(void);

/************************************** 全局变量 *******************************************/

/*电位计入场验收*/
unsigned char adcbuff[4096]; 
ring_buffer rb_adc_rx;							/*环形队列*/

/*舵机出厂检测1模式*/
unsigned char steering_gear_mode1[4096]; 
ring_buffer stgear_rx;							/*环形队列*/

/*舵机出厂检测2模式*/
unsigned char steering_gear_mode2[4096]; 
ring_buffer stgear_rxmode2;					/*环形队列*/

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim6;

SPI_HandleTypeDef hspi2;
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

/* Private function prototypes -----------------------------------------------*/
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_SPI2_Init(void);

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
		High7
4.	任务类型
		20ms轮询执行
*/
void AppTaskLwIPPro(void *argument);
const osThreadAttr_t ThreadLwIPPro_Attr = 		
{
	.name = "osRtxLwIPProThread",
	.attr_bits = osThreadDetached, 
	.priority = osPriorityHigh7,
	.stack_size = 2048,
};
osThreadId_t ThreadIdTaskLwIPPro = NULL;	

/*	
AppTaskSteering_gear
1.	任务名：
		舵机出厂检测模式一
2.	任务工作：
		取出舵机板卡发送的串口队列数据，并解析，传回传角度 和 SPI数据 放入 环形队列stgear_rx中，
		将舵机板卡回传的参数纠正数据转成UDP数据传输给上位机
3.	优先级：
		High4
4.	任务类型
		堵塞性任务

*/
void AppTaskSteering_gear(void *argument);
const osThreadAttr_t ThreadSteering_gear_Attr = 
{
	.name = "osRtxSteering_gearThread",
	.attr_bits = osThreadDetached, 
	.priority = osPriorityHigh4,
	.stack_size = 4096,
};
osThreadId_t ThreadIdTaskSteering_gear = NULL;


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


/***
AppTaskGetDataAndUDP
1.	任务名：		
2.	任务工作：		
		1.	根据event_Sampling_ID 切换模式， 电位计入场验收，环形队列数据量满足800后发送
		2.	根据event_Sampling_ID 切换模式，舵机角度检测1，环形队列数据量满足800后发送，
		3.	根据event_Sampling_ID 切换模式，舵机角度检测2，环形队列数据量满足800后发送
3.	优先级：		
		High3
5.	任务类型
		200ms轮询执行
***/
void AppTaskGetDataAndUDP(void *argument);		
const osThreadAttr_t ThreadGetDataAndUDP_Attr = 
{
	.name = "osRtxGetDataAndUDPThread",
	.attr_bits = osThreadDetached, 
	.priority = osPriorityHigh3,
	.stack_size = 12288,
};
osThreadId_t ThreadIdTaskGetDataAndUDP = NULL;


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


/************************************** 定时器 *******************************************/
void timer_Periodic_App(void *argument);
osTimerId_t  timerID_Periodic = NULL;
const osTimerAttr_t timer_Periodic_Attr = { .name = "periodic timer", };

/************************************** 消息队列 *******************************************/
osMessageQueueId_t msgQueue_UdpResData;				/*接收UDP数据*/
const osMessageQueueAttr_t UdpResData_Attr ={.name = "UdpResData",};

osMessageQueueId_t msgQueue_rxuart1Data; 			/*串口1缓存数据*/		
const osMessageQueueAttr_t rxuart1Data_Attr ={.name = "rxuart1Data", };
/************************************** 事件标志位 **********************************************/

/* 网线插拔事件标志组属性 */
const osEventFlagsAttr_t event_cable_Attr = { .name = "event_cable", };
osEventFlagsId_t event_cable_ID = NULL;

/* 网口获取数据接 事件标志组属性 */
const osEventFlagsAttr_t event_UdpGetData_Attr = { .name = "event_UdpGetData", };
osEventFlagsId_t event_UdpGetData_ID = NULL;

/* 采样控制 */
const osEventFlagsAttr_t event_Sampling_Attr = { .name = "event_Sampling", };
osEventFlagsId_t event_Sampling_ID = NULL;

/* HZ model 模式 */
const osEventFlagsAttr_t event_HzMode_Attr = { .name = "event_HzMode", };
osEventFlagsId_t event_HzMode_ID = NULL;

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
	GPIO_LEDInitialization();
	bsp_InitGpio();	
	bsp_LED_InitGpio();
	MX_SPI2_Init();
	MX_DMA_Init();
	MX_ADC1_Init();
	
	MX_USART1_UART_Init();
	
	/**UART DMA config**/	
	__HAL_UART_ENABLE_IT(&husart1, UART_IT_IDLE);	/* 开启串口空闲中断 */
	#define UART_RX_MODE_DMA	/* 开启串口接收 大小缓冲区最大长度rxSize */	
	#ifdef UART_RX_MODE_DMA
	HAL_UART_Receive_DMA(&husart1, g_tBspUsart1.pRxBuf, g_tBspUsart1.rxSize);
	#else
	HAL_UART_Receive_IT(&husart1, g_tBspUsart1.pRxBuf, g_tBspUsart1.rxSize);
	#endif

	bsp_Spi_Cs_InitGpio();
	RB_Init(&rb_adc_rx , 			adcbuff , 						4096);	/* 电位计入场验收模式 队列*/
	RB_Init(&stgear_rx , 			steering_gear_mode1 , 4096);	/* 舵机模式1模式	队列*/
	RB_Init(&stgear_rxmode2 , steering_gear_mode2 , 4096);	/* 舵机模式2模式	队列*/
	
	EventRecorderInitialize(EventRecordAll, 1U);						/* 初始化EventRecorder并开启 */
	EventRecorderStart();		
	
	msgQueue_UdpResData  = 	osMessageQueueNew(12 , sizeof(struct Get_UdPData) , &UdpResData_Attr); /* 接收UDP数据消息队列*/		
	msgQueue_rxuart1Data  = osMessageQueueNew(12 , sizeof(BspUart_t) , 					&rxuart1Data_Attr);	
	event_cable_ID 		= 	osEventFlagsNew(&event_cable_Attr);					
	event_Sampling_ID = 	osEventFlagsNew(&event_Sampling_Attr);
	event_HzMode_ID 	= 	osEventFlagsNew(&event_HzMode_Attr);	
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
	
	/**	向上位机打包数据任务	**/
	ThreadIdTaskGetDataAndUDP = osThreadNew(AppTaskGetDataAndUDP, NULL, &ThreadGetDataAndUDP_Attr); 	
	tick += usFrequency + 500;                          
	osDelayUntil(tick);
	
	/*舵机数据解包任务*/
	ThreadIdTaskSteering_gear = osThreadNew(AppTaskSteering_gear, NULL, &ThreadSteering_gear_Attr);		
	tick += usFrequency + 500;                          
	osDelayUntil(tick);	

	/* 这个定时器浪费了很多资源在需要关闭的时候关闭它 */
	timerID_Periodic = osTimerNew(timer_Periodic_App , osTimerPeriodic , NULL , &timer_Periodic_Attr);	
	SEGGER_RTT_ConfigUpBuffer(0, "RTTUP", NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);			/* 配置通道0，上行配置*/
	SEGGER_RTT_ConfigDownBuffer(0, "RTTDOWN", NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);	/* 配置通道0，下行配置*/	
	SEGGER_RTT_SetTerminal(0);
	SEGGER_RTT_printf(0, "the project is runing \r\n");
	
	tick = osKernelGetTickCount(); 
	while(1){
		tick += usFrequency;                          
		osDelayUntil(tick);
		osThreadExit();
	}
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void){
  /* USER CODE BEGIN ADC1_Init 0 */
  /* USER CODE END ADC1_Init 0 */
  ADC_ChannelConfTypeDef sConfig = {0};
  /* USER CODE BEGIN ADC1_Init 1 */
  /* USER CODE END ADC1_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)*/
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK){
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.*/
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK){
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */
  /* USER CODE END ADC1_Init 2 */
}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void){
  /* USER CODE BEGIN TIM6_Init 0 */
  /* USER CODE END TIM6_Init 0 */
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  /* USER CODE BEGIN TIM6_Init 1 */
  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 4199;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
//  htim6.Init.Period = 1;
	htim6.Init.Period = 20;	// 1ms
//	htim6.Init.Period = 10;	// 0.5ms
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK){
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK){
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */
  /* USER CODE END TIM6_Init 2 */
}


/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
void MX_SPI2_Init(void){
  /* USER CODE BEGIN SPI2_Init 0 */
  /* USER CODE END SPI2_Init 0 */
  /* USER CODE BEGIN SPI2_Init 1 */
  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_HARD_OUTPUT;
  //hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
	hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK){
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */
  /* USER CODE END SPI2_Init 2 */
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

