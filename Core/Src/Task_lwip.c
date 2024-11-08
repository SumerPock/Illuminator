/**
  ******************************************************************************
  * @file           : Task_lwip.c
  * @brief          : 
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  *
  ******************************************************************************
  */

#include "bsp.h"




/**
  * @brief 	网线插拔状态，非堵塞性
  * @retval 
  */
void AppTaskEthCheck(void *argument)
{
	const uint16_t usFrequency = 500;	
	unsigned int tick = 0;
	tick = osKernelGetTickCount();
	osStatus_t os_Status;
	unsigned char strct = 0;
	while(1)
	{
		ethernetif_check_link(&netif);	/*DUBUG模式下可以正常识别，但断电后不进入DUBUG模式，无法识别*/
		if(netif_is_link_up(&netif)) 		/*只有当网线插上后才建立UDP链接*/
		{
			osEventFlagsClear(event_cable_ID , 0x02U);
			osEventFlagsSet(event_cable_ID , 0x01U);
			udp_echoserver_init();
			osThreadExit(); 
		}
		else
		{
			osEventFlagsClear(event_cable_ID , 0x01U);
			osEventFlagsSet(event_cable_ID , 0x02U);
		}	
		tick += usFrequency;    /* 相对延迟 */                      
		osDelayUntil(tick);	
	}	
}



/**
  * @brief 	udp数据接收轮询，非堵塞性
  * @retval 
  */
void AppTaskLwIPPro(void *argument)
{
	const uint16_t usFrequency = 100;/* 延迟周期 */
	unsigned int tick = 0;
	tick = osKernelGetTickCount(); 	/* 获取当前时间 */
	osStatus_t os_Status;
	while(1){
		ethernetif_poll(&netif);
		tick += usFrequency;    			/* 相对延迟 */                      
		osDelayUntil(tick);	
	}
}


/**
  * @brief 
  * @retval udp数据接收任务，使用堵塞性任务
  * 
  */
void AppTaskUdpNetWork(void *argument){
	const uint16_t usFrequency = 25;
	unsigned int tick = 0;
	tick = osKernelGetTickCount();
	osStatus_t os_Status;
	struct Get_UdPData mygetudpdata = {0};
	unsigned char buffer[30] = {0};	
	struct preciseCoding PreCod = {0};
	int osStatus = 0;
//	struct Parameter_Steering_gear pssentdata = {0};
	while(1)
	{
		os_Status = osMessageQueueGet(msgQueue_UdpResData, &mygetudpdata, NULL,	osWaitForever);    											
		if(osOK == os_Status)
		{
			if(mygetudpdata.nUdpD_size == mygetudpdata.arrUdp_index[0])
			{
				switch(mygetudpdata.arrUdp_index[1])
				{
				case 1:					
				break;
					
				case 2://固定脉冲频率
				PreCod.Frequency = mygetudpdata.arrUdp_index[3] | mygetudpdata.arrUdp_index[4] << 8;
				int narr = 0;
				int nRedress = 0;
					switch(mygetudpdata.arrUdp_index[2])
					{
					case 0://装订		
					setpar.pulseCount = 0;					
					setpar.pulseCountSet = PreCod.Frequency;
					PreCod.CodCycle = mygetudpdata.arrUdp_index[5] | mygetudpdata.arrUdp_index[6] << 8;
					nRedress = PreCod.CodCycle / 1000;/*矫正计算*/
					narr =  (PreCod.CodCycle * 84 - 1) - arrRedress[nRedress];
					Update_Timer_Arr(narr);
					setpar.nflagGpio = 1;	
					if(HAL_TIM_Base_Start_IT(&htim2)!= HAL_OK){
					}	
					break;
							
					case 1://控制
					if(mygetudpdata.arrUdp_index[3] == 0x01){
					}else if(mygetudpdata.arrUdp_index[3] == 0x02){
						if(HAL_TIM_Base_Stop_IT(&htim2)!= HAL_OK){
						}									
					}
					break;
						
					case 2://查询
					break;		
					}
				break;

				case 4:
				if(mygetudpdata.arrUdp_index[2] == 1)//控制
				{//控制
					if(mygetudpdata.arrUdp_index[3] == 1)
						ThreadIdTaskSetPWM = osThreadNew(AppTaskSetPWM, NULL, &ThreadSetPWM_Attr);	
					else if(mygetudpdata.arrUdp_index[3] == 2)
						osEventFlagsSet(event_PWMTaskFlag_ID , 0x01U << 0); 
				}
				else if(mygetudpdata.arrUdp_index[2] == 0)
				{//装订
				}
				else if(mygetudpdata.arrUdp_index[2] ==2)
				{//查询
				}
				break;

				}
			}
		}	
	}
}



