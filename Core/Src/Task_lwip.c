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
	const uint16_t usFrequency = 250;	
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
	const uint16_t usFrequency = 20;/* 延迟周期 */
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
  * @retval None
  */
void convertSetUdpDataToByteArray(struct SETUDPDATA *data, unsigned char *byteArray) 
{
    memcpy(byteArray, data, sizeof(struct SETUDPDATA));
}


/**
  * @brief 
  * @retval udp数据打包发送任务
  */
void AppTaskGetDataAndUDP(void *argument){	
	unsigned int tick = 0;										
	unsigned short usFrequency = 200; 	
	struct Steering_gear_Mode1 send_gear_mode1 = {0};	
	send_gear_mode1.SgUdpHand[0] = 0xcc;							//Hand
	send_gear_mode1.SgUdpHand[1] = 0xdd;
	send_gear_mode1.SgUdpNum = 0x00;									//序号
	send_gear_mode1.SgUdpID = 0xa1;										/*id = 舵机指令1 舵机角度传输反馈*/
	send_gear_mode1.SgUdpSampling_Rate[0] = 0xE8;			//采样率 1000
	send_gear_mode1.SgUdpSampling_Rate[1] = 0X03;
	send_gear_mode1.SgUdpSampling_Number[0] = 0xC8;		//采样数 200
	send_gear_mode1.SgUdpSampling_Number[1] = 0x00;

	struct SETUDPDATA setUdpData = {0};					/*电位计入场验收*/
	setUdpData.ucSetUdpHand[0] = 0xcc;					//hand
	setUdpData.ucSetUdpHand[1] = 0xdd;
	setUdpData.ucSetUdploop = 0x00;		
	setUdpData.ucSetUdpID = 0x04;									/*id = 电位计入场验收*/
	setUdpData.ucSetUdpSampling_Rate[0] = 0xd0; 	//2000
	setUdpData.ucSetUdpSampling_Rate[1] = 0x07;
	
	setUdpData.ucSetUdpSampling_Number[0] = 0xC8;	//200
	setUdpData.ucSetUdpSampling_Number[1] = 0x00;		

	struct SETUDPDATA setUdpData_mode2 = {0};			/*舵机指令2*/
	setUdpData_mode2.ucSetUdpHand[0] = 0xcc;
	setUdpData_mode2.ucSetUdpHand[1] = 0xdd;
	
	setUdpData_mode2.ucSetUdploop = 0x00;
	setUdpData_mode2.ucSetUdpID = 0xA2;						/*id = 舵机指令2 HZ运动模式*/
	setUdpData_mode2.ucSetUdpSampling_Rate[0] = 0xd0;
	setUdpData_mode2.ucSetUdpSampling_Rate[1] = 0x07;
	
	setUdpData_mode2.ucSetUdpSampling_Number[0] = 0xC8;
	setUdpData_mode2.ucSetUdpSampling_Number[1] = 0x00;			
	
	static unsigned char loopdata = 0;		
	osStatus_t os_Status; 
	int flag = 0;
	while(1)
	{
		unsigned int numberint = 0;
		os_Status = osEventFlagsGet(event_Sampling_ID);		
		
		if(os_Status == 0x08)
		{			
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
			#ifdef	DEBUGPR
				SEGGER_RTT_SetTerminal(1);
				SEGGER_RTT_printf(0, "the mode is clocse \r\n");	
			/**在这加退出任务的意思是什么 ？ 分析了一下，这个任务不需要关闭**/
			//osThreadExit();
			#endif
		}
		else if(os_Status == 0x01)//电位计入场验收，SPI和ADC采集模式
		{
			numberint = RB_Get_Length(&rb_adc_rx);
			if(numberint >= 800)
			{
				if(RING_BUFFER_SUCCESS == (RB_Read_String(&rb_adc_rx, setUdpData.getdata, 800)))
				{
					loopdata++;
					setUdpData.ucSetUdploop = loopdata;
					unsigned char UDPSend[808] = {0};
					convertSetUdpDataToByteArray(&setUdpData, UDPSend);	
					udp_echoclient_send(UDPSend, sizeof(struct SETUDPDATA));
					numberint = RB_Get_Length(&rb_adc_rx);
					
					HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_7);
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
					#ifdef	DEBUGPR
						SEGGER_RTT_SetTerminal(1);
						SEGGER_RTT_printf(0, "send udp mode 1 \r\n");	
					#endif					
				}
			}
		}
		
		else if(os_Status == 0x02)//舵机调教1，上位机固定角度设置
		{
			numberint = RB_Get_Length(&stgear_rx);
			if(numberint >= 800)
			{
				if(RING_BUFFER_SUCCESS == (RB_Read_String(&stgear_rx, send_gear_mode1.SgUdpSteering_gear_spi, 800)))
				{
					loopdata++;
					send_gear_mode1.SgUdpNum = loopdata;
					unsigned char UDPSend[808] = {0};
					memmove(UDPSend, &send_gear_mode1, 808);
					udp_echoclient_send(UDPSend, 808);
					numberint = RB_Get_Length(&stgear_rx);
					
					HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_3);
					HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);		
					#ifdef	DEBUGPR
						SEGGER_RTT_SetTerminal(1);
						SEGGER_RTT_printf(0, "send udp mode 2 \r\n");		
					#endif	
				}			
			}	
		}
		
		else if(os_Status == 0x04)//舵机调教2，下位机正弦运动模式
		{
			numberint = RB_Get_Length(&stgear_rxmode2);
			if(numberint >= 800)
			{
				if(RING_BUFFER_SUCCESS == (RB_Read_String(&stgear_rxmode2, setUdpData_mode2.getdata, 800)))
				{
					loopdata++;
					setUdpData_mode2.ucSetUdploop = loopdata;
					unsigned char UDPSend[808] = {0};
					convertSetUdpDataToByteArray(&setUdpData_mode2, UDPSend);	
					udp_echoclient_send(UDPSend, sizeof(struct SETUDPDATA));
					numberint = RB_Get_Length(&stgear_rxmode2);
					
					HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_4);
					HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
					#ifdef	DEBUGPR
						SEGGER_RTT_SetTerminal(1);
						SEGGER_RTT_printf(0, "send udp mode 3 \r\n");	
					#endif						
				}
			}
		}		
		else
		{
			#ifdef	DEBUGPR
				SEGGER_RTT_SetTerminal(1);
				SEGGER_RTT_printf(0, "the mode is run Unknown \r\n");		
			#endif
		}
		
		tick += usFrequency;                          
		osDelayUntil(tick);
	}		
}

/**
  * @brief 
  * @retval udp数据接收任务，使用堵塞性任务
  * 
  */
void AppTaskUdpNetWork(void *argument){
	const uint16_t usFrequency = 100;
	unsigned int tick = 0;
	tick = osKernelGetTickCount();
	osStatus_t os_Status;
	struct GETUDPDATA GetUdpData = {0};	
	struct Get_UdPData mygetudpdata = {0};
	unsigned char buffer[30] = {0};	
	struct STEER_UART_DATA GetsteerData = {0};
	struct Parameter_Steering_gear pssentdata = {0};
	while(1)
	{
		os_Status = osMessageQueueGet(msgQueue_UdpResData, &mygetudpdata, NULL,	osWaitForever);    											
		if(osOK == os_Status)
		{
			if(mygetudpdata.ucudpdata[0] == 0xAA && mygetudpdata.ucudpdata[1] == 0xBB)
			{
				switch(mygetudpdata.ucudpdata[3])
				{				
					case 0xA1:/*上位机下发舵机角度控制*/
						GetsteerData.steer_Hand = mygetudpdata.ucudpdata[8] | mygetudpdata.ucudpdata[9] << 8;				//帧头
						GetsteerData.steer_len =  mygetudpdata.ucudpdata[10];																				//帧长
						GetsteerData.steer_loop =  mygetudpdata.ucudpdata[11] |  mygetudpdata.ucudpdata[12] << 8;		//帧序号
						GetsteerData.steering_gear1 = mygetudpdata.ucudpdata[13] | mygetudpdata.ucudpdata[14] << 8;	//接收舵机指令1
						GetsteerData.steering_gear2 = mygetudpdata.ucudpdata[15] | mygetudpdata.ucudpdata[16] << 8;	//接收舵机指令2
						GetsteerData.chickdata = mygetudpdata.ucudpdata[17] | mygetudpdata.ucudpdata[18] << 8;			//接收效验码
						GetsteerData.enddata = mygetudpdata.ucudpdata[19];																					//接收帧尾

						buffer[0] = GetsteerData.steer_Hand;
						buffer[1] = GetsteerData.steer_Hand >> 8;					
						buffer[2] = GetsteerData.steer_len;				
						buffer[3] = GetsteerData.steer_loop;
						buffer[4] = GetsteerData.steer_loop >> 8;					
						buffer[5] =	GetsteerData.steering_gear1;
						buffer[6] =	GetsteerData.steering_gear1 >> 8;					
						buffer[7] = GetsteerData.steering_gear2;
						buffer[8] = GetsteerData.steering_gear2 >> 8;					
						buffer[9] = GetsteerData.chickdata;
						buffer[10] = GetsteerData.chickdata >> 8;
						buffer[11] = GetsteerData.enddata;			
						BSP_UART_Transmit_DMA(&husart1 , buffer , 12);		
						#ifdef	DEBUGPR
							SEGGER_RTT_SetTerminal(1);
							SEGGER_RTT_printf(0, "Set Servo angle \r\n");		
						#endif
						break;
						
					case 0xA2:/*舵机出厂检测2 - 正弦运动模式*/
						#ifdef	DEBUGPR
							SEGGER_RTT_SetTerminal(1);
							SEGGER_RTT_printf(0, "Set Servo angle HZ = %d \r\n" , mygetudpdata.ucudpdata[8]);		
						#endif
						switch(mygetudpdata.ucudpdata[8]){
							case 0x00:	//停止
								setGear_0HZ_0D();
								break;
							case 0x01: //1HZ 3
								setGear_1HZ_3D();
								if(osThreadGetState(AppTaskARMSteer_gear) != osThreadReady){
										ThreadIdTaskARMSteer_gear = osThreadNew(AppTaskARMSteer_gear, NULL, &ThreadARMSteer_gear_Attr);
								}
								else{
									#ifdef	DEBUGPR
										SEGGER_RTT_SetTerminal(1);
										SEGGER_RTT_printf(0, "tne AppTaskARMSteer_gear is error 2\r\n");	
									#endif									
								}
								break;
							case 0x02: //1Hz 5																		
								setGear_1HZ_5D();
								if(osThreadGetState(AppTaskARMSteer_gear) != osThreadReady){
										ThreadIdTaskARMSteer_gear = osThreadNew(AppTaskARMSteer_gear, NULL, &ThreadARMSteer_gear_Attr);
								}
								else{
									#ifdef	DEBUGPR
										SEGGER_RTT_SetTerminal(1);
										SEGGER_RTT_printf(0, "tne AppTaskARMSteer_gear is error 2\r\n");	
									#endif										
								}
								break;
							case 0x03: //1Hz 10
								setGear_1HZ_10D();	
								if(osThreadGetState(AppTaskARMSteer_gear) != osThreadReady){
										ThreadIdTaskARMSteer_gear = osThreadNew(AppTaskARMSteer_gear, NULL, &ThreadARMSteer_gear_Attr);
								}
								else{
									#ifdef	DEBUGPR
										SEGGER_RTT_SetTerminal(1);
										SEGGER_RTT_printf(0, "tne AppTaskARMSteer_gear is error 2\r\n");		
									#endif									
								}							
								break;
							case 0x04: //1Hz 15						
								setGear_1HZ_15D();
								if(osThreadGetState(AppTaskARMSteer_gear) != osThreadReady){
										ThreadIdTaskARMSteer_gear = osThreadNew(AppTaskARMSteer_gear, NULL, &ThreadARMSteer_gear_Attr);
								}
								else{
									#ifdef	DEBUGPR
										SEGGER_RTT_SetTerminal(1);
										SEGGER_RTT_printf(0, "tne AppTaskARMSteer_gear is error 2\r\n");	
									#endif									
								}
								break;
							case 0x05: //3Hz 3°10
								setGear_3HZ_3D();
								if(osThreadGetState(AppTaskARMSteer_gear) != osThreadReady){
										ThreadIdTaskARMSteer_gear = osThreadNew(AppTaskARMSteer_gear, NULL, &ThreadARMSteer_gear_Attr);
								}
								else{
									#ifdef	DEBUGPR
										SEGGER_RTT_SetTerminal(1);
										SEGGER_RTT_printf(0, "tne AppTaskARMSteer_gear is error 2\r\n");
									#endif										
								}							
								break;
							case 0x06: //3Hz 5°
								setGear_3HZ_5D();
								if(osThreadGetState(AppTaskARMSteer_gear) != osThreadRunning){
									ThreadIdTaskARMSteer_gear = osThreadNew(AppTaskARMSteer_gear, NULL, &ThreadARMSteer_gear_Attr);
								}
								else{
									#ifdef	DEBUGPR
										SEGGER_RTT_SetTerminal(1);
										SEGGER_RTT_printf(0, "tne AppTaskARMSteer_gear is error 2\r\n");	
									#endif										
								}
								break;
							case 0x07: //3Hz 10°
								setGear_3HZ_10D();
								if(osThreadGetState(AppTaskARMSteer_gear) != osThreadRunning){
									ThreadIdTaskARMSteer_gear = osThreadNew(AppTaskARMSteer_gear, NULL, &ThreadARMSteer_gear_Attr);
								}
								else{
									#ifdef	DEBUGPR
										SEGGER_RTT_SetTerminal(1);
										SEGGER_RTT_printf(0, "tne AppTaskARMSteer_gear is error 2\r\n");	
									#endif										
								}							
								break;
							case 0x08: //3Hz 15°
								setGear_3HZ_15D();
								if(osThreadGetState(AppTaskARMSteer_gear) != osThreadRunning){
									ThreadIdTaskARMSteer_gear = osThreadNew(AppTaskARMSteer_gear, NULL, &ThreadARMSteer_gear_Attr);
								}
								else{
									#ifdef	DEBUGPR
										SEGGER_RTT_SetTerminal(1);
										SEGGER_RTT_printf(0, "tne AppTaskARMSteer_gear is error 2\r\n");
									#endif										
								}							
								break;
							case 0x09: //5Hz 3°
								setGear_5HZ_3D();
								if(osThreadGetState(AppTaskARMSteer_gear) != osThreadRunning){
									ThreadIdTaskARMSteer_gear = osThreadNew(AppTaskARMSteer_gear, NULL, &ThreadARMSteer_gear_Attr);
								}
								else{
									#ifdef	DEBUGPR
										SEGGER_RTT_SetTerminal(1);
										SEGGER_RTT_printf(0, "tne AppTaskARMSteer_gear is error 2\r\n");	
									#endif											
								}							
								break;
							case 0x0a: //5Hz 5°
								setGear_5HZ_5D();
								if(osThreadGetState(AppTaskARMSteer_gear) != osThreadRunning){
									ThreadIdTaskARMSteer_gear = osThreadNew(AppTaskARMSteer_gear, NULL, &ThreadARMSteer_gear_Attr);
								}
								else{
									#ifdef	DEBUGPR
										SEGGER_RTT_SetTerminal(1);
										SEGGER_RTT_printf(0, "tne AppTaskARMSteer_gear is error 2\r\n");
									#endif										
								}							
								break;
							case 0x0b: //5Hz 10°
								setGear_5HZ_10D();
								if(osThreadGetState(AppTaskARMSteer_gear) != osThreadRunning){
									ThreadIdTaskARMSteer_gear = osThreadNew(AppTaskARMSteer_gear, NULL, &ThreadARMSteer_gear_Attr);
								}
								else{
									#ifdef	DEBUGPR
										SEGGER_RTT_SetTerminal(1);
										SEGGER_RTT_printf(0, "tne AppTaskARMSteer_gear is error 2\r\n");
									#endif										
								}							
								break;
							case 0x0c: //5Hz 15°
								setGear_5HZ_15D();
								if(osThreadGetState(AppTaskARMSteer_gear) != osThreadRunning){
									ThreadIdTaskARMSteer_gear = osThreadNew(AppTaskARMSteer_gear, NULL, &ThreadARMSteer_gear_Attr);
								}
								else{
									#ifdef	DEBUGPR
										SEGGER_RTT_SetTerminal(1);
										SEGGER_RTT_printf(0, "tne AppTaskARMSteer_gear is error 2\r\n");	
									#endif										
								}							
								break;		
							case 0x0d: //7Hz 3°
								setGear_7HZ_3D();
								if(osThreadGetState(AppTaskARMSteer_gear) != osThreadRunning){
									ThreadIdTaskARMSteer_gear = osThreadNew(AppTaskARMSteer_gear, NULL, &ThreadARMSteer_gear_Attr);
								}
								else{
									#ifdef	DEBUGPR
										SEGGER_RTT_SetTerminal(1);
										SEGGER_RTT_printf(0, "tne AppTaskARMSteer_gear is error 2\r\n");
									#endif									
								}							
								break;	
							case 0x0e: //7Hz 5°
								setGear_7HZ_5D();
								if(osThreadGetState(AppTaskARMSteer_gear) != osThreadRunning){
									ThreadIdTaskARMSteer_gear = osThreadNew(AppTaskARMSteer_gear, NULL, &ThreadARMSteer_gear_Attr);
								}
								else{
									#ifdef	DEBUGPR
										SEGGER_RTT_SetTerminal(1);
										SEGGER_RTT_printf(0, "tne AppTaskARMSteer_gear is error 2\r\n");	
									#endif	
								}							
								break;
							case 0x0f: //7Hz 10°
								setGear_7HZ_10D();
								if(osThreadGetState(AppTaskARMSteer_gear) != osThreadRunning){
									ThreadIdTaskARMSteer_gear = osThreadNew(AppTaskARMSteer_gear, NULL, &ThreadARMSteer_gear_Attr);
								}
								else{
									#ifdef	DEBUGPR
										SEGGER_RTT_SetTerminal(1);
										SEGGER_RTT_printf(0, "tne AppTaskARMSteer_gear is error 2\r\n");	
									#endif	
								}							
								break;	
							case 0x10: //7Hz 15°
								setGear_7HZ_15D();
								if(osThreadGetState(AppTaskARMSteer_gear) != osThreadRunning){
									ThreadIdTaskARMSteer_gear = osThreadNew(AppTaskARMSteer_gear, NULL, &ThreadARMSteer_gear_Attr);
								}
								else{
									#ifdef	DEBUGPR
										SEGGER_RTT_SetTerminal(1);
										SEGGER_RTT_printf(0, "tne AppTaskARMSteer_gear is error 2\r\n");	
									#endif										
								}							
								break;	
						case 0x11: //9Hz 3°
								setGear_9HZ_3D();
								if(osThreadGetState(AppTaskARMSteer_gear) != osThreadRunning){
									ThreadIdTaskARMSteer_gear = osThreadNew(AppTaskARMSteer_gear, NULL, &ThreadARMSteer_gear_Attr);
								}
								else{
									#ifdef	DEBUGPR
										SEGGER_RTT_SetTerminal(1);
										SEGGER_RTT_printf(0, "tne AppTaskARMSteer_gear is error 2\r\n");	
									#endif										
								}						
								break;	
						case 0x12: //9Hz 5°
								setGear_9HZ_5D();
								if(osThreadGetState(AppTaskARMSteer_gear) != osThreadRunning){
									ThreadIdTaskARMSteer_gear = osThreadNew(AppTaskARMSteer_gear, NULL, &ThreadARMSteer_gear_Attr);
								}
								else{
									#ifdef	DEBUGPR
										SEGGER_RTT_SetTerminal(1);
										SEGGER_RTT_printf(0, "tne AppTaskARMSteer_gear is error 2\r\n");	
									#endif										
								}						
								break;		
						case 0x13: //9Hz 10°
								setGear_9HZ_10D();
								if(osThreadGetState(AppTaskARMSteer_gear) != osThreadRunning){
									ThreadIdTaskARMSteer_gear = osThreadNew(AppTaskARMSteer_gear, NULL, &ThreadARMSteer_gear_Attr);
								}
								else{
									#ifdef	DEBUGPR
										SEGGER_RTT_SetTerminal(1);
										SEGGER_RTT_printf(0, "tne AppTaskARMSteer_gear is error 2\r\n");	
									#endif									
								}						
								break;	
						case 0x14: //9Hz 15°
								setGear_9HZ_15D();
								if(osThreadGetState(AppTaskARMSteer_gear) != osThreadRunning){
									ThreadIdTaskARMSteer_gear = osThreadNew(AppTaskARMSteer_gear, NULL, &ThreadARMSteer_gear_Attr);
								}
								else{
									#ifdef	DEBUGPR
										SEGGER_RTT_SetTerminal(1);
										SEGGER_RTT_printf(0, "tne AppTaskARMSteer_gear is error 2\r\n");
									#endif
								}						
								break;								
							default:
								break;
						}
						break;

					case 0x00://停止采样
						if(osTimerIsRunning(timerID_Periodic) == 1){
							osTimerStop(timerID_Periodic);	//停止电位计入场验收，定时器
						}
						/* 清除SPI , ADC缓存 */
						int numberint =  RB_Get_Length(&rb_adc_rx);
						unsigned char tempdata[800] = {0};		
						if(numberint != 0){
							RB_Write_String(&rb_adc_rx, tempdata, 800);
						}					
						osEventFlagsClear(event_Sampling_ID , 0x01U << 0);
						osEventFlagsClear(event_Sampling_ID , 0x01U << 1);
						osEventFlagsClear(event_Sampling_ID , 0x01U << 2);
						osEventFlagsSet(event_Sampling_ID , 0x01U << 3); 	
						#ifdef	DEBUGPR
							SEGGER_RTT_SetTerminal(1);
							SEGGER_RTT_printf(0, "Stop sampling \r\n");		
						#endif
						break;
					
					case 0x01://电位计入场验收
						if(osTimerIsRunning(timerID_Periodic) == 0){
							osTimerStart(timerID_Periodic , 1U);
							#ifdef	DEBUGPR
								SEGGER_RTT_SetTerminal(1);
								SEGGER_RTT_printf(0, "open Poten mode\r\n");	
							#endif
						}	
						else
						{
							#ifdef	DEBUGPR
								SEGGER_RTT_SetTerminal(1);
								SEGGER_RTT_printf(0, "the timer is not runing \r\n");	
							#endif							
						}
						osEventFlagsClear(event_Sampling_ID , 0x01U << 1);
						osEventFlagsClear(event_Sampling_ID , 0x01U << 2);
						osEventFlagsClear(event_Sampling_ID , 0x01U << 3);		
						osEventFlagsSet(event_Sampling_ID , 0x01U << 0); 
						break;
					
					case 0x02://开始舵机模式1	
						osEventFlagsClear(event_Sampling_ID , 0x01U << 0);
						osEventFlagsClear(event_Sampling_ID , 0x01U << 2);
						osEventFlagsClear(event_Sampling_ID , 0x01U << 3);
						osEventFlagsSet(event_Sampling_ID , 0x01U << 1);
						#ifdef	DEBUGPR
							SEGGER_RTT_SetTerminal(1);
							SEGGER_RTT_printf(0, "open mode 1 \r\n");
						#endif						
						break;
					
					case 0x03://开始舵机模式2
						osEventFlagsClear(event_Sampling_ID , 0x01U << 0);
						osEventFlagsClear(event_Sampling_ID , 0x01U << 1);
						osEventFlagsClear(event_Sampling_ID , 0x01U << 3);
						osEventFlagsSet(event_Sampling_ID , 0x01U << 2); 
						#ifdef	DEBUGPR
							SEGGER_RTT_SetTerminal(1);
							SEGGER_RTT_printf(0, "open mode 2 \r\n");	
						#endif	
						break;
					
					case 0xD1:/*舵机参数纠正*/
						pssentdata.ucpsdata[0] =  mygetudpdata.ucudpdata[8];
						pssentdata.ucpsdata[1] =  mygetudpdata.ucudpdata[9];
						pssentdata.ucpsdata[2] =  mygetudpdata.ucudpdata[10]; //命令类型
						pssentdata.ucpsdata[3] =  mygetudpdata.ucudpdata[11];	
						pssentdata.ucpsdata[4] =  mygetudpdata.ucudpdata[12];	//BCD1
						pssentdata.ucpsdata[5] =  mygetudpdata.ucudpdata[13]; //BCD2
						pssentdata.ucpsdata[6] =  mygetudpdata.ucudpdata[14]; //传感器系数
						pssentdata.ucpsdata[7] =  mygetudpdata.ucudpdata[15];
						pssentdata.ucpsdata[8] =  mygetudpdata.ucudpdata[16];
						pssentdata.ucpsdata[9] =  mygetudpdata.ucudpdata[17];
						pssentdata.ucpsdata[10] =  mygetudpdata.ucudpdata[18];//传感器零位
						pssentdata.ucpsdata[11] =  mygetudpdata.ucudpdata[19];
						pssentdata.ucpsdata[12] =  mygetudpdata.ucudpdata[20];//Float 1
						pssentdata.ucpsdata[13] =  mygetudpdata.ucudpdata[21];
						pssentdata.ucpsdata[14] =  mygetudpdata.ucudpdata[22];
						pssentdata.ucpsdata[15] =  mygetudpdata.ucudpdata[23];
						pssentdata.ucpsdata[16] =  mygetudpdata.ucudpdata[24];//Float 2
						pssentdata.ucpsdata[17] =  mygetudpdata.ucudpdata[25];
						pssentdata.ucpsdata[18] =  mygetudpdata.ucudpdata[26];
						pssentdata.ucpsdata[19] =  mygetudpdata.ucudpdata[27];
						pssentdata.ucpsdata[20] =  mygetudpdata.ucudpdata[28];//Float 3
						pssentdata.ucpsdata[21] =  mygetudpdata.ucudpdata[29];
						pssentdata.ucpsdata[22] =  mygetudpdata.ucudpdata[30];
						pssentdata.ucpsdata[23] =  mygetudpdata.ucudpdata[31];
						pssentdata.ucpsdata[24] =  mygetudpdata.ucudpdata[32];//校验和
						BSP_UART_Transmit_DMA(&husart1 , pssentdata.ucpsdata , 25);
						break;
					
					default:
						break;
				}
			}
		}	

		
	}
}

void setGear_0HZ_0D(void)
{
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 0);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 1);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 2);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 3);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 4);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 5);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 6);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 7);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 8);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 9);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 10);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 11);						
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 12);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 13);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 14);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 15);		
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 16);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 17);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 18);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 19);								
	osEventFlagsSet(event_HzMode_ID , 0x00000001U << 20); //stop
}

//1HZ 3
void setGear_1HZ_3D(void)
{
	//osEventFlagsClear(event_HzMode_ID , 0x00000001U << 0);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 1);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 2);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 3);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 4);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 5);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 6);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 7);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 8);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 9);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 10);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 11);						
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 12);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 13);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 14);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 15);		
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 16);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 17);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 18);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 19);					
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 20); //stop						
	osEventFlagsSet(event_HzMode_ID , 0x00000001U << 0); //1
}
//1HZ 5
void setGear_1HZ_5D(void)
{
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 0);
	//osEventFlagsClear(event_HzMode_ID , 0x00000001U << 1);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 2);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 3);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 4);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 5);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 6);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 7);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 8);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 9);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 10);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 11);						
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 12);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 13);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 14);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 15);		
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 16);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 17);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 18);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 19);								
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 20); //stop

	osEventFlagsSet(event_HzMode_ID , 0x00000001U << 1); //1			
}
//1HZ 10
void setGear_1HZ_10D(void)
{
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 0);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 1);
	//osEventFlagsClear(event_HzMode_ID , 0x00000001U << 2);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 3);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 4);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 5);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 6);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 7);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 8);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 9);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 10);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 11);						
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 12);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 13);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 14);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 15);		
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 16);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 17);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 18);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 19);	
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 20); //stop

	osEventFlagsSet(event_HzMode_ID , 0x00000001U << 2);	
}
//1HZ 15
void setGear_1HZ_15D(void)
{
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 0);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 1);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 2);
	//sEventFlagsClear(event_HzMode_ID , 0x00000001U << 3);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 4);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 5);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 6);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 7);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 8);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 9);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 10);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 11);						
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 12);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 13);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 14);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 15);		
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 16);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 17);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 18);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 19);	
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 20); //stop

	osEventFlagsSet(event_HzMode_ID , 0x00000001U << 3);	
}


//3HZ 3
void setGear_3HZ_3D(void)
{
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 0);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 1);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 2);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 3);
	//osEventFlagsClear(event_HzMode_ID , 0x00000001U << 4);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 5);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 6);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 7);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 8);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 9);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 10);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 11);						
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 12);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 13);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 14);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 15);		
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 16);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 17);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 18);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 19);	
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 20); //stop

	osEventFlagsSet(event_HzMode_ID , 0x00000001U << 4);	
}
//3HZ 5
void setGear_3HZ_5D(void)
{
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 0);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 1);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 2);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 3);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 4);

	//osEventFlagsClear(event_HzMode_ID , 0x00000001U << 5);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 6);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 7);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 8);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 9);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 10);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 11);						
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 12);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 13);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 14);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 15);		
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 16);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 17);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 18);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 19);	
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 20); //stop

	osEventFlagsSet(event_HzMode_ID , 0x00000001U << 5);	
}
//3HZ 10
void setGear_3HZ_10D(void)
{
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 0);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 1);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 2);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 3);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 4);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 5);
	//osEventFlagsClear(event_HzMode_ID , 0x00000001U << 6);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 7);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 8);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 9);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 10);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 11);						
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 12);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 13);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 14);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 15);		
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 16);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 17);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 18);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 19);	
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 20); //stop

	osEventFlagsSet(event_HzMode_ID , 0x00000001U << 6);	
}
//3HZ 15
void setGear_3HZ_15D(void)
{
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 0);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 1);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 2);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 3);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 4);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 5);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 6);
	//osEventFlagsClear(event_HzMode_ID , 0x00000001U << 7);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 8);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 9);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 10);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 11);						
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 12);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 13);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 14);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 15);		
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 16);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 17);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 18);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 19);	
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 20); //stop

	osEventFlagsSet(event_HzMode_ID , 0x00000001U << 7);	
}


//5HZ 3
void setGear_5HZ_3D(void)
{
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 0);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 1);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 2);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 3);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 4);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 5);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 6);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 7);
	//osEventFlagsClear(event_HzMode_ID , 0x00000001U << 8);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 9);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 10);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 11);						
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 12);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 13);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 14);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 15);		
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 16);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 17);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 18);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 19);	
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 20); //stop

	osEventFlagsSet(event_HzMode_ID , 0x00000001U << 8);	
}
//5HZ 5
void setGear_5HZ_5D(void)
{
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 0);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 1);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 2);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 3);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 4);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 5);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 6);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 7);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 8);

	//osEventFlagsClear(event_HzMode_ID , 0x00000001U << 9);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 10);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 11);						
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 12);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 13);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 14);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 15);		
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 16);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 17);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 18);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 19);	
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 20); //stop

	osEventFlagsSet(event_HzMode_ID , 0x00000001U << 9);	
}
//5HZ 10
void setGear_5HZ_10D(void)
{
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 0);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 1);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 2);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 3);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 4);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 5);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 6);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 7);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 8);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 9);
	//osEventFlagsClear(event_HzMode_ID , 0x00000001U << 10);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 11);						
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 12);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 13);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 14);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 15);		
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 16);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 17);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 18);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 19);	
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 20); //stop

	osEventFlagsSet(event_HzMode_ID , 0x00000001U << 10);	
}
//5HZ 15
void setGear_5HZ_15D(void)
{
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 0);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 1);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 2);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 3);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 4);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 5);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 6);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 7);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 8);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 9);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 10);
	//osEventFlagsClear(event_HzMode_ID , 0x00000001U << 11);						
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 12);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 13);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 14);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 15);		
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 16);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 17);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 18);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 19);	
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 20); //stop

	osEventFlagsSet(event_HzMode_ID , 0x00000001U << 11);	
}


//7HZ 3
void setGear_7HZ_3D(void)
{
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 0);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 1);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 2);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 3);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 4);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 5);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 6);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 7);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 8);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 9);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 10);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 11);						
	//osEventFlagsClear(event_HzMode_ID , 0x00000001U << 12);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 13);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 14);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 15);		
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 16);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 17);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 18);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 19);	
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 20); //stop

	osEventFlagsSet(event_HzMode_ID , 0x00000001U << 12);	
}
//7HZ 5
void setGear_7HZ_5D(void)
{
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 0);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 1);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 2);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 3);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 4);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 5);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 6);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 7);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 8);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 9);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 10);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 11);						
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 12);

	//osEventFlagsClear(event_HzMode_ID , 0x00000001U << 13);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 14);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 15);		
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 16);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 17);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 18);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 19);	
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 20); //stop

	osEventFlagsSet(event_HzMode_ID , 0x00000001U << 13);	
}
//7HZ 10
void setGear_7HZ_10D(void)
{
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 0);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 1);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 2);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 3);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 4);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 5);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 6);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 7);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 8);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 9);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 10);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 11);						
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 12);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 13);
	//osEventFlagsClear(event_HzMode_ID , 0x00000001U << 14);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 15);		
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 16);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 17);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 18);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 19);	
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 20); //stop

	osEventFlagsSet(event_HzMode_ID , 0x00000001U << 14);	
}
//7HZ 15
void setGear_7HZ_15D(void)
{
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 0);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 1);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 2);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 3);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 4);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 5);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 6);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 7);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 8);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 9);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 10);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 11);						
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 12);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 13);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 14);
	//osEventFlagsClear(event_HzMode_ID , 0x00000001U << 15);		
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 16);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 17);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 18);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 19);	
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 20); //stop

	osEventFlagsSet(event_HzMode_ID , 0x00000001U << 15);	
}

//9HZ 3
void setGear_9HZ_3D(void)
{
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 0);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 1);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 2);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 3);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 4);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 5);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 6);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 7);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 8);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 9);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 10);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 11);						
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 12);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 13);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 14);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 15);		
	//osEventFlagsClear(event_HzMode_ID , 0x00000001U << 16);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 17);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 18);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 19);	
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 20); //stop

	osEventFlagsSet(event_HzMode_ID , 0x00000001U << 16);
}
//9HZ 5
void setGear_9HZ_5D(void)
{
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 0);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 1);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 2);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 3);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 4);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 5);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 6);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 7);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 8);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 9);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 10);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 11);						
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 12);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 13);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 14);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 15);		
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 16);
	//osEventFlagsClear(event_HzMode_ID , 0x00000001U << 17);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 18);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 19);	
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 20); //stop

	osEventFlagsSet(event_HzMode_ID , 0x00000001U << 17);	
}
//9HZ 10
void setGear_9HZ_10D(void)
{
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 0);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 1);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 2);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 3);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 4);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 5);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 6);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 7);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 8);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 9);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 10);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 11);						
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 12);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 13);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 14);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 15);		
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 16);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 17);

	//osEventFlagsClear(event_HzMode_ID , 0x00000001U << 18);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 19);	
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 20); //stop

	osEventFlagsSet(event_HzMode_ID , 0x00000001U << 18);	
}
//9HZ 15
void setGear_9HZ_15D(void)
{
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 0);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 1);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 2);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 3);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 4);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 5);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 6);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 7);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 8);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 9);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 10);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 11);						
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 12);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 13);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 14);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 15);		
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 16);
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 17);

	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 18);
	//osEventFlagsClear(event_HzMode_ID , 0x00000001U << 19);	
	osEventFlagsClear(event_HzMode_ID , 0x00000001U << 20); //stop

	osEventFlagsSet(event_HzMode_ID , 0x00000001U << 19);	
}


