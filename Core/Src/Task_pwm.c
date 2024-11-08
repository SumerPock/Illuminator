#include "bsp.h"


void AppTaskSetPWM(void *argument)
{	
	osStatus_t os_Status;
	osStatus_t os_StatusQueue;
	
	struct preciseCoding PreCod = {0 , 0};
	
	//GpioToogleSetPar setpar = {839999, 2519999, 0, 0, 0, 0};
	/*队列取数*/
	//os_Status = osMessageQueueGet(msgQueue_PreFreData , &PreCod , NULL , 0);   	
	//GetBaseCoding.baseCodFrequency
	
	uint16_t usFrequency = 1;
	uint32_t tick;
	tick = osKernelGetTickCount();
	
//	setpar.nflagGpio = 1;				// 	 1 代表状态机开始
//	setpar.pulseCountSet = -1;  // 	-1 代表无限次
//	setpar.pulseCountSet = PreCod.Frequency;
//	setpar.arrtimelow = (PreCod.CodCycle * 10) * 84 -1;
	
//	Update_Timer_Arr(setpar.arrtimelow);
	
//	printf("PreCod.CodCycle = %d \r\n" , PreCod.CodCycle);
//	printf("setpar.arrtimelow = %d \r\n" , setpar.arrtimelow);
//	GpioToogleSet(&setpar);			//	先执行一遍开启
//	event_Time2Flag_ID	= osEventFlagsNew(&event_Time2Flag_Attr);	
//	unsigned char onoff = 0;
//	printf("the ThreadIdTaskSetPWM on is ok\r\n");
//	timerID_Periodic = osTimerNew(timer_Periodic_App , osTimerPeriodic , NULL , &timer_Periodic_Attr);	
//	
//	
//	GpioToogleSetPar setpar = {839999, 2519999, 0, 0, 0, 0};
	
	unsigned int 		nRandomFg = 0;
	unsigned int 		nTimeEnd = 0;
	unsigned int 		nRandomCode = 1;
//	unsigned short 	arrRandomCode[5] = {5000 , 10000 , 20000 , 25000 , 30000};
//	unsigned short 	arrRandomCode_count = 6; //伪随机数据个数

	unsigned short 	arrRandomCode[5] = {5000 , 10000 , 15000};
	unsigned short 	arrRandomCode_count = 4; //伪随机数据个数

	int narr = 0;
	int nRedress = 0;
	int nRedressD = 0;
	
	/*计算脉冲次数*/
	setpar.pulseCount = 0;
	/*脉冲次数*/
	setpar.pulseCountSet = 2;
	/*周期*/
	PreCod.CodCycle = arrRandomCode[nRandomCode];
	/*偏差计算*/
	nRedress = PreCod.CodCycle / 1000;
	narr =  (PreCod.CodCycle * 84 - 1) - arrRedress[nRedress];
	
	printf("nRedressD = %d \r\n" , nRedressD);
	printf("nRedress = %d \r\n" , arrRedress[nRedress]);
	printf("PreCod.CodCycle = %d \r\n" , narr);
	
	Update_Timer_Arr(narr);
	setpar.nflagGpio = 1;	
	if(HAL_TIM_Base_Start_IT(&htim2)!= HAL_OK){
	}	
	
	while(1)
	{
		nRandomFg = osEventFlagsGet(event_PWMTaskFlag_ID);
		nTimeEnd = osEventFlagsGet(event_Time2Flag_ID);	
		if(nRandomFg == 1)
		{
			nRandomFg = 0;
			osEventFlagsClear(event_PWMTaskFlag_ID , 0x00000001U);
			if(HAL_TIM_Base_Stop_IT(&htim2)!= HAL_OK){
			}
			osThreadExit();
		}
		else
		{
		}
		
		if(nTimeEnd == 1)
		{
			nTimeEnd = 0;
			osEventFlagsClear(event_Time2Flag_ID , 0x00000001U);

			
			if(nRandomCode < arrRandomCode_count)
			{			
				/*计算脉冲次数*/
				setpar.pulseCount = 0;
				/*脉冲次数*/
				setpar.pulseCountSet = 2;
				/*周期*/
				PreCod.CodCycle = arrRandomCode[nRandomCode - 1];
				/*偏差计算*/
				nRedress = PreCod.CodCycle / 1000;
				narr =  (PreCod.CodCycle * 84 - 1) - arrRedress[nRedress];
				
				printf("nRedressD = %d \r\n" , nRedressD);
				printf("nRedress = %d \r\n" , arrRedress[nRedress]);
				printf("PreCod.CodCycle = %d \r\n" , narr);
				
				Update_Timer_Arr(narr);
				setpar.nflagGpio = 1;	
				if(HAL_TIM_Base_Start_IT(&htim2)!= HAL_OK){
				}					
				++nRandomCode;				
			}
			else
			{
				nRandomCode = 1;
				
				/*计算脉冲次数*/
				setpar.pulseCount = 0;
				/*脉冲次数*/
				setpar.pulseCountSet = 2;
				/*周期*/
				PreCod.CodCycle = arrRandomCode[nRandomCode - 1];
				/*偏差计算*/
				nRedress = PreCod.CodCycle / 1000;
				narr =  (PreCod.CodCycle * 84 - 1) - arrRedress[nRedress];

				printf("nRedressD = %d \r\n" , nRedressD);
				printf("nRedress = %d \r\n" , arrRedress[nRedress]);
				printf("PreCod.CodCycle = %d \r\n" , narr);

				Update_Timer_Arr(narr);
				setpar.nflagGpio = 1;	
				if(HAL_TIM_Base_Start_IT(&htim2)!= HAL_OK){
				}	
				++nRandomCode;
			}

		}
		tick += usFrequency;                          
		osDelayUntil(tick);		
	}
}