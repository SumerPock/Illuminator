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


unsigned short arrLCDClearArea[20][4]= {
{5, 5 , 230 , 50},	//����

{5, 60 ,  230 , 60  + 30},	//1
{5, 95 ,  230 , 95  + 30},	//2
{5, 130 , 230 , 130 + 30},	//3
{5, 165 , 230 , 165 + 26}, 	//4

{5, 206 , 230 , 205 + 20},	//�������α���
{5, 230 , 230 , 230 + 20},	//����״̬
{5, 255 , 230 , 255 + 20},	//��Դ״̬
{5, 280 , 230 , 280 + 20},	//����ʱ��

{85, 206 , 230 , 205 + 20},	//α���
{85, 230 , 230 , 230 + 20},	//����״̬
{85, 255 , 230 , 255 + 20},	//��Դ״̬
{85, 280 , 230 , 280 + 20},	//ʱ��



{10, 40 , 260 , 185},//α����������


};

/************************************** ȫ�ֱ��� *******************************************/

///*��λ���볡����*/
//unsigned char adcbuff[4096]; 
//ring_buffer rb_adc_rx;							/*���ζ���*/
///*����������1ģʽ*/
//unsigned char steering_gear_mode1[4096]; 
//ring_buffer stgear_rx;							/*���ζ���*/
///*����������2ģʽ*/
//unsigned char steering_gear_mode2[4096]; 
//ring_buffer stgear_rxmode2;					/*���ζ���*/

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim6;


/* Private function prototypes -----------------------------------------------*/
static void MX_GPIO_Init(void);


/************************************** ���� *******************************************/

/*	
AppTaskEthCheck
1.	��������		
		���߲�μ��
2.	���������ݣ�		
		��������Ƿ���룬����������������,Ȼ����˳�����
3.	���ȼ���		 
		High1
4.	��������
		250ms��ѯִ��
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
1.	��������		
		LWIP�������ݽ�����ѯ
2.	��������	
		��ѯ������������
3.	���ȼ���		
		High
4.	��������
		20ms��ѯִ��
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
1.	��������	
2.	��������	
		1.	������λ������Ƕȿ���ָ���ͨ��DMA����
		2.	������λ������ѡ�˶�ģʽ����������������������˶�ָ��
		3.	������λ����ֹͣ����ģʽ
		4.	������λ���ĵ�λ���볡����ģʽ��
		5.	������λ���Ķ����������ģʽ��ͨ��DMA����
3.	���ȼ���
		High6
4.	��������
		����������

*/
void AppTaskUdpNetWork(void *argument);					
const osThreadAttr_t ThreadUdpNetWork_Attr =
{
	.name = "osRtxUdpNetWorkThread",
	.attr_bits = osThreadDetached, 
	.priority = osPriorityHigh6,
	.stack_size = 4096 * 4,
};
osThreadId_t ThreadIdTaskUdpNetWork = NULL;




/**
AppTaskStart
1.	��������
			��������
2.	��������
3.	���ȼ���
			High7
6.	��������
			ִ�й���ر�
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
1.	��������
2.	��������
			�·������˶�ģʽ���ݣ�������ǰ���͵����ҽǶȺͽ��յ��Ķ�������Ƕȴ��������
3.	���ȼ���
			High5
4.	��������
			����������
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
1.	��������
2.	��������
			α�������
3.	���ȼ���
			High7
4.	��������
**/
void AppTaskSetPWM(void *argument);
const osThreadAttr_t ThreadSetPWM_Attr =
{
	.name = "osRtxSetPWMThread",
	.attr_bits = osThreadDetached, 
	.priority = osPriorityHigh7,
	.stack_size = 2048,
};
osThreadId_t ThreadIdTaskSetPWM = NULL;

/**
AppTaskUIMode


**/
void AppTaskUIMode(void *argument);
const osThreadAttr_t ThreadUIMode_Attr =
{
	.name = "osRtxUIModeThread",
	.attr_bits = osThreadDetached, 
	.priority = osPriorityHigh5,
	.stack_size = 2048,
};
osThreadId_t ThreadIdTaskUIMode = NULL;



/************************************** ��ʱ�� *******************************************/
void timer_Periodic_App(void *argument);
osTimerId_t  timerID_Periodic = NULL;
const osTimerAttr_t timer_Periodic_Attr = { .name = "periodic timer", };

/************************************** ��Ϣ���� *******************************************/
osMessageQueueId_t msgQueue_UdpResData;				/*����UDP����*/
const osMessageQueueAttr_t UdpResData_Attr ={.name = "UdpResData",};

osMessageQueueId_t msgQueue_rxuart1Data; 			/*����1��������*/		
const osMessageQueueAttr_t rxuart1Data_Attr ={.name = "rxuart1Data", };

osMessageQueueId_t msgQueue_PreFreData; /*���侫ȷƵ������Ϣ*/
const osMessageQueueAttr_t PreFreData_Attr ={.name = "PreFreData", };

/************************************** �¼���־λ **********************************************/

/* ���߲���¼���־������ */
const osEventFlagsAttr_t event_cable_Attr = { .name = "event_cable", };
osEventFlagsId_t event_cable_ID = NULL;

/*PWM�������*/
const osEventFlagsAttr_t event_PWMTaskFlag_Attr = {.name = "event_PwmTaskFlag"};
osEventFlagsId_t event_PWMTaskFlag_ID = NULL;

/*��ʱ��2*/
const osEventFlagsAttr_t event_Time2Flag_Attr = {.name = "event_Time2Flag",};
osEventFlagsId_t event_Time2Flag_ID = NULL;


GpioToogleSetPar setpar = {839999, 2519999, 0, 0, 0, 0};

void gpiodemo(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	__HAL_RCC_GPIOA_CLK_ENABLE();	
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	
	__HAL_RCC_GPIOB_CLK_ENABLE();
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

	GPIO_InitStruct.Pin = GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);	

//	GPIO_InitStruct.Pin  = GPIO_PIN_0;
//	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//	GPIO_InitStruct.Pull = GPIO_PULLUP;
//	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
//	
//	GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_5;
//	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//	GPIO_InitStruct.Pull = GPIO_PULLUP;
//	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//	
//		GPIO_InitStruct.Pin = GPIO_PIN_13 | GPIO_PIN_15;
//	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//	GPIO_InitStruct.Pull = GPIO_PULLUP;
//	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
//	
//		GPIO_InitStruct.Pin = GPIO_PIN_7;
//	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//	GPIO_InitStruct.Pull = GPIO_PULLUP;
//	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
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
	MX_DMA_Init();
	EventRecorderInitialize(EventRecordAll, 1U);	
	EventRecorderStart();	
	HAL_SuspendTick();
	osKernelInitialize();                                  
	ThreadIdStart = osThreadNew(AppTaskStart, NULL, &ThreadStart_Attr);  
	osKernelStart();
}

/**
  * @brief  ��������
  * @retval None
  */
void AppTaskStart(void *argument)
{
	const uint16_t usFrequency = 100;
	uint32_t tick;	
	HAL_ResumeTick();
	MX_TIM2_Init();	
	
	/*��ʼ��Ĭ�Ϲرն�ʱ���жϹܽ�*/
//	if(HAL_TIM_Base_Start_IT(&htim2) != HAL_OK){
//	}	
	
//	setpar.nflagGpio = 1;

	gpiodemo();

	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

	bsp_InitSPIBus();			/* ����SPI���� */	
	LCD_Init();						/* SPI LCD��ʼ��*/
	LCD_Clear(DARKBLUE);	
	DISPlay_Title();
	DISPlay_Function_1(0);
	DISPlay_Function_2(0);
	DISPlay_Function_3(0);
	DISPlay_Function_4(1);

	
	DISPlay_Rectangle();
	DISPlay_Function_5(0);
	DISPlay_Function_6(0);	
	DISPlay_Function_7(0);
	DISPlay_Function_8(0);

	ConfigSP330(U1, RS422);
	ConfigSP330(U26, RS422);
	
//	MX_DMA_Init();
	MX_USART1_UART_Init();
	/**UART DMA config**/
	//HAL_UART_Transmit(&husart1 , (uint8_t *)"this is uart1 send data\r\n", 24, 1000);	
	__HAL_UART_ENABLE_IT(&husart1 , UART_IT_IDLE);	/* �������ڿ����ж� */
	#define UART_RX_MODE_DMA	/* �������ڽ��� ��С��������󳤶�rxSize */	
	#ifdef UART_RX_MODE_DMA
	HAL_UART_Receive_DMA(&husart1 , g_tBspUsart1.pRxBuf , g_tBspUsart1.rxSize);
	#else
	HAL_UART_Receive_IT(&huart1, g_tBspUart1.pRxBuf, g_tBspUart1.rxSize);
	#endif
	
//	MX_UART4_UART_Init();
//	//HAL_UART_Transmit(&huart4, (uint8_t *)"this is uart4 send data!\r\n", 24, 1000);
//	__HAL_UART_ENABLE_IT(&huart4, UART_IT_IDLE);	/* �������ڿ����ж� */
//	#define UART_RX_MODE_DMA											/* �������ڽ��� ��С��������󳤶�rxSize */
//	#ifdef UART_RX_MODE_DMA
//	HAL_UART_Receive_DMA(&huart4, g_tBspUart4.pRxBuf, g_tBspUart4.rxSize);
//	#else
//	HAL_UART_Receive_IT(&huart4, g_tBspUart4.pRxBuf, g_tBspUart4.rxSize);
//	#endif

	SEGGER_RTT_ConfigUpBuffer(0, "RTTUP", NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);			/* ����ͨ��0����������*/
	SEGGER_RTT_ConfigDownBuffer(0, "RTTDOWN", NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);	/* ����ͨ��0����������*/	
	SEGGER_RTT_SetTerminal(1);
	SEGGER_RTT_printf(0, "SEGGER_RTT_GetKey \r\n");	

//	DISPlay_image_park(initialize , center);
//	DISPlay_self_test();
//	UI_CilckFun(1);
//	DISPlay_DrawCircle_Break();	

//	SEGGER_RTT_ConfigUpBuffer(0, "RTTUP", NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);			/* ����ͨ��0����������*/
//	SEGGER_RTT_ConfigDownBuffer(0, "RTTDOWN", NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);	/* ����ͨ��0����������*/	
//	SEGGER_RTT_SetTerminal(0);
//	SEGGER_RTT_printf(0, "the project is runing \r\n");

	msgQueue_UdpResData  = osMessageQueueNew(12 , sizeof(struct Get_UdPData) , &UdpResData_Attr); /* ����UDP������Ϣ����*/	
	msgQueue_PreFreData  = osMessageQueueNew(12 , sizeof(struct preciseCoding) , &PreFreData_Attr);	
	if(net_init() != 0){
		//printf("lwip is config fild \r\n");
	}
	
//	/* LWIP���� udp��ѯ���� */
//	ThreadIdTaskLwIPPro = osThreadNew(AppTaskLwIPPro, NULL, &ThreadLwIPPro_Attr);	
//	tick += usFrequency + 500;                          
//	osDelayUntil(tick);	
	
//	/* ���߲��״̬���*/	
//	ThreadIdTaskEthCheck = osThreadNew(AppTaskEthCheck, NULL, &ThreadEthCheck_Attr); 	
//	tick += usFrequency + 500;                          
//	osDelayUntil(tick);		

	/* UDP �������ݽ�������*/
//	ThreadIdTaskUdpNetWork = osThreadNew(AppTaskUdpNetWork, NULL, &ThreadUdpNetWork_Attr);	
//	tick += usFrequency + 500;                          
//	osDelayUntil(tick);
	
	ThreadIdTaskUIMode = osThreadNew(AppTaskUIMode, NULL, &ThreadUIMode_Attr);	
	tick += usFrequency + 500;                          
	osDelayUntil(tick);	
	

	/*α�����������Ҫ�ر�ʱ�رգ�����ϵͳ����*/
	event_PWMTaskFlag_ID = osEventFlagsNew(&event_PWMTaskFlag_Attr);	
	/*��ʱ������жϱ�־*/
	event_Time2Flag_ID	= osEventFlagsNew(&event_Time2Flag_Attr);	
	tick = osKernelGetTickCount(); 
	
//	Update_Timer_Arr(419999);
//	if(HAL_TIM_Base_Start_IT(&htim2)!= HAL_OK){
//	}  arrLCDClearArea
//	/*�������	230 50*/
//	LCD_ClearArea(arrLCDClearArea[0][0] , arrLCDClearArea[0][1] , arrLCDClearArea[0][2] , arrLCDClearArea[0][3] , DARKBLUE);
//	LCD_ClearArea(arrLCDClearArea[1][0] , arrLCDClearArea[1][1] , arrLCDClearArea[1][2] , arrLCDClearArea[1][3] , DARKBLUE);
//	LCD_ClearArea(arrLCDClearArea[2][0] , arrLCDClearArea[2][1] , arrLCDClearArea[2][2] , arrLCDClearArea[2][3] , DARKBLUE);
//	LCD_ClearArea(arrLCDClearArea[3][0] , arrLCDClearArea[3][1] , arrLCDClearArea[3][2] , arrLCDClearArea[3][3] , DARKBLUE);
//	LCD_ClearArea(arrLCDClearArea[4][0] , arrLCDClearArea[4][1] , arrLCDClearArea[4][2] , arrLCDClearArea[4][3] , DARKBLUE);	
	
	//LCD_ClearArea(arrLCDClearArea[11][0] , arrLCDClearArea[11][1] , arrLCDClearArea[11][2] , arrLCDClearArea[11][3] , DARKBLUE);	
	
//	DisPlayfunrandom();	
//	float arrRandom[20] = {
//	110.456 , 11.234 , 30.124 , 110.123 , 
//	50.369  , 63.123 , 67.456 , 80.123  ,
//	90.123  , 92.123
//	};	//����
//	SetRandom(10 , &arrRandom[0]);
//	DisPlayfunPCCommand();
//	DisPlayfundebug();
	
	
	//LCD_ClearArea(arrLCDClearArea[13][0] , arrLCDClearArea[13][1] , arrLCDClearArea[13][2] , arrLCDClearArea[13][3] , DARKBLUE);
	//DISPlay_FunNum(0);
	while(1)
	{
		tick += usFrequency;                          
		osDelayUntil(tick);
		osThreadExit();
	}
}


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
						//HAL_GPIO_WritePin(GPIOE , GPIO_PIN_10 , GPIO_PIN_SET);
						HAL_GPIO_WritePin(GPIOC , GPIO_PIN_10 , GPIO_PIN_SET);
						bsp_DelayUS(5);	
						//HAL_GPIO_WritePin(GPIOE , GPIO_PIN_10 , GPIO_PIN_RESET);
						HAL_GPIO_WritePin(GPIOC , GPIO_PIN_10 , GPIO_PIN_RESET);
					}
					else 
					{
						osEventFlagsSet(event_Time2Flag_ID , 0x01U << 0);	
						/*stop*/
						if(HAL_TIM_Base_Stop_IT(&htim2)!= HAL_OK){	
						}else{
							setpar.pulseCount = 0;
						}						
					}
				}else{
					//���޴�
					//HAL_GPIO_TogglePin(GPIOE , GPIO_PIN_10);
					HAL_GPIO_TogglePin(GPIOC , GPIO_PIN_10);
					bsp_DelayUS(5);	
					//HAL_GPIO_WritePin(GPIOE , GPIO_PIN_10 , GPIO_PIN_RESET);	
					HAL_GPIO_WritePin(GPIOC , GPIO_PIN_10 , GPIO_PIN_RESET);
				}
				break;
		}	
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

