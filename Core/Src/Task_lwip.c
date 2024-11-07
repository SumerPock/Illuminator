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
			for(int i = 0 ; i < mygetudpdata.nUdpD_size ; i++)
			{
				printf("GetTcpData.arrData[%d] = %d \r\n", i , mygetudpdata.arrUdp_index[i]);
//				SEGGER_RTT_SetTerminal(0);
//				SEGGER_RTT_printf(0, "GetTcpData.arrData[%d] = %d \r\n", i , mygetudpdata.arrUdp_index[i]);
			}
			/*数据解析*/
			if(mygetudpdata.nUdpD_size == mygetudpdata.arrUdp_index[0])
			{
				switch(mygetudpdata.arrUdp_index[1])
				{
					case 1:
//						BSP_UART_Transmit_DMA(&husart3 , &GetTcpData.arrData[2] , GetTcpData.arrData[0] - 4);
//						SEGGER_RTT_SetTerminal(0);
//						SEGGER_RTT_printf(0, "fen funis xieyizhuanfa \r\n");						
						break;
					case 2://固定脉冲频率
						PreCod.Frequency = mygetudpdata.arrUdp_index[2] | mygetudpdata.arrUdp_index[3] << 8;
						if(PreCod.Frequency == 0)
						{
							//停止,无需解析周期数据了
							//osEventFlagsSet(event_PWMTaskFlag_ID , 0x01U << 0);	
							//关闭定时器中断
							if(HAL_TIM_Base_Stop_IT(&htim2)!= HAL_OK){
							}
						}
						else if(PreCod.Frequency == -1)
						{
							int narr = 0;
							//osStatus = osThreadGetState(ThreadIdTaskSetPWM);
							PreCod.CodCycle = mygetudpdata.arrUdp_index[4] | mygetudpdata.arrUdp_index[5] << 8;
							narr =  (PreCod.CodCycle * 10) * 84 -1;;
							printf("PreCod.CodCycle = %d \r\n" , narr);
							setpar.nflagGpio = 1;
							
							Update_Timer_Arr(narr);
							//第一次出，无限次
							if(HAL_TIM_Base_Start_IT(&htim2)!= HAL_OK){
							}									
							
							//if(osStatus == osThreadRunning || osStatus == osThreadBlocked)
							{
//								//停止,无需解析周期数据了
//								osEventFlagsSet(event_PWMTaskFlag_ID , 0x01U << 0);
//								//无限次
								
//								osMessageQueuePut(msgQueue_PreFreData , &PreCod , NULL , NULL); 
//								printf("on ThreadIdTaskSetPWM \r\n");
//								ThreadIdTaskSetPWM = osThreadNew(AppTaskSetPWM, NULL, &ThreadSetPWM_Attr);
//								osEventFlagsSet(event_PWMTaskFlag_ID , 0x00U << 0);	

									//第二次出无限次
//									if(HAL_TIM_Base_Stop_IT(&htim2)!= HAL_OK){
//									}
//									if(HAL_TIM_Base_Start_IT(&htim2)!= HAL_OK){
//									}		
							}
							//else if(osStatus == osThreadError)
							{
						
//								PreCod.CodCycle = mygetudpdata.arrUdp_index[4] | mygetudpdata.arrUdp_index[5] << 8;
//								osMessageQueuePut(msgQueue_PreFreData , &PreCod , NULL , NULL); 
//								printf("on ThreadIdTaskSetPWM \r\n");
//								ThreadIdTaskSetPWM = osThreadNew(AppTaskSetPWM, NULL, &ThreadSetPWM_Attr);									
							}
							
						}
					
//						SEGGER_RTT_SetTerminal(0);
//						SEGGER_RTT_printf(0, "fen funis neirongbu \r\n");	
						break;
					case 3:
//						SEGGER_RTT_SetTerminal(0);
//						SEGGER_RTT_printf(0, "fen funis neitongbuweisuiji \r\n");	
						break;
					case 4:
//						SEGGER_RTT_SetTerminal(0);
//						SEGGER_RTT_printf(0, "fen funis jidianqi \r\n");	
						break;
				}
			}
			
//			udp_echoclient_send(buffer , 30);
//			if(mygetudpdata.ucudpdata[0] == 0xAA && mygetudpdata.ucudpdata[1] == 0xBB)
//			{
//				
//			}
		}	
	}
}



