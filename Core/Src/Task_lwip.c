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
	const uint16_t usFrequency = 20; /* 延迟周期 */
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
	struct preciseCoding PreCod = {0 , 0};
	int osStatus = 0;
	
	/**底下这些打开后程序运行一会就死机了**/
	/*内同步普通编码模式-返回脉冲数*/
	unsigned char arrSendNum[] = {0x08 , 0x02 , 0x02 , 0x01, 0x00 , 0x00 , 0xee , 0xbb};	
	
	/*内同步普通编码模式-返回周期数*/
	unsigned char arrSendCycle[] = {0x08 , 0x02 , 0x02 , 0x02, 0x00 , 0x00 , 0xee , 0xbb};
	
	/*内同步普通编码模式-返回脉冲状态*/
	unsigned char arrSendPulseState[] = {0x07 , 0x02 , 0x02 , 0x03, 0x00 , 0xee , 0xbb};	
	
	/*内同步普通编码模式-返回全部参数*/
	unsigned char arrSendAll[] = { 0x0B , 0x02 , 0x02 , 0x0f , 
																 0x00 , 0x00 , 0x00 , 0x00 , 0x00 ,
																 0xee , 0xbb	};
	
	/*伪随机模式查询返回*/															 
	unsigned char arrRandomDataReturn[] = {0x0b , 0x03 , 0x02 , 0x01 , 
																	 0x00 , 0x00 , 
																	 0x00 , 0x00 , 
																	 0x00 , 0x00 ,
																	 0xEE , 0XBB	};
	
	unsigned int npulseNum = 0;						//脉冲数量
	unsigned int ncycle = 0;							//脉冲周期
	unsigned int npulseState = 2;	//脉冲状态

	/**读取**/
	unsigned char  nrandomGroup = 0;				//伪随机编码组								 
	unsigned short nrandomSum   = 0;				//伪随机编码总数
	unsigned short nrandomIndex	= 0;				//伪随机编码当前索引
	unsigned short nrandomData = 0;					//伪随机编码值
	struct RANDOM	gRamdomD = {0 , 0 , 0, 0, {0 , 0}};	

	/**查询**/
	unsigned char  nRrandomGroup = 0;				//伪随机编码组								 
	unsigned short nRrandomSum   = 0;				//伪随机编码总数
	unsigned short nRrandomIndex	= 0;				//伪随机编码当前索引
	unsigned short nRrandomData = 0;					//伪随机编码值
																 
	//二维数组用于存储伪随机数据
																 
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
				
				/**	固定脉冲频率	**/
				case 2:
				PreCod.Frequency = mygetudpdata.arrUdp_index[3] | mygetudpdata.arrUdp_index[4] << 8;
				int narr = 0;
				int nRedress = 0;
				switch(mygetudpdata.arrUdp_index[2])
				{
					/***	数据装订	***/
					case 0:	
					setpar.pulseCount = 0;					
					setpar.pulseCountSet = PreCod.Frequency;
					PreCod.CodCycle = mygetudpdata.arrUdp_index[5] | mygetudpdata.arrUdp_index[6] << 8 | mygetudpdata.arrUdp_index[7] << 16;
					if(PreCod.CodCycle >= CYCLELIMITMIN && PreCod.CodCycle <= CYCLELIMITMAX)
						PreCod.CodCycle = PreCod.CodCycle;
					else
						PreCod.CodCycle = CYCLELIMITDEF;
					nRedress = PreCod.CodCycle / 1000;/*矫正计算*/
					/*自动计算*/
					narr =  (PreCod.CodCycle * 84 - 1) - arrRedress[nRedress];
					Update_Timer_Arr(narr);
					/*上位机传值*/
//					unsigned short test = mygetudpdata.arrUdp_index[7] | mygetudpdata.arrUdp_index[8] << 8;
//					narr =  (PreCod.CodCycle * 84 - 1) - test;
//					narr =  (PreCod.CodCycle * 84 - 1);
//					Update_Timer_Arr(narr);					
					setpar.nflagGpio = 1;
					break;
					
					/***	脉冲控制	***/
					case 1:
					if(mygetudpdata.arrUdp_index[3] == 0x01)
					{
						if(PreCod.CodCycle != 0 && setpar.pulseCountSet != 0)
						{
							npulseState = 1;
							if(HAL_TIM_Base_Start_IT(&htim2)!= HAL_OK){
							}								
						}
					}
					else if(mygetudpdata.arrUdp_index[3] == 0x02)
					{
						if(PreCod.CodCycle != 0)
						{
							npulseState = 2;
							if(HAL_TIM_Base_Stop_IT(&htim2)!= HAL_OK){
							}									
						}			
					}
					break;
					
					/***	脉冲查询	***/
					case 2:				
					if(mygetudpdata.arrUdp_index[3] == 0x01)
					{//脉冲数		 pulseCountSet				
						npulseNum = setpar.pulseCountSet;
						arrSendNum[4] = npulseNum;
						arrSendNum[5] = npulseNum >> 8;
						unsigned char i = 0;
						udp_echoclient_send(arrSendNum , 8);
					}
					if(mygetudpdata.arrUdp_index[3] == 0x02)
					{//周期						
						ncycle = PreCod.CodCycle;
						arrSendCycle[4] = ncycle;
						arrSendCycle[5] = ncycle >> 8;
						arrSendCycle[6] = ncycle >> 16;
						arrSendCycle[7] = ncycle >> 24;
						unsigned char i = 0;
						udp_echoclient_send(arrSendCycle , 10);
					}
					if(mygetudpdata.arrUdp_index[3] == 0x03)
					{//脉冲状态npulseState
						arrSendPulseState[4] = npulseState;
						unsigned char i = 0;
						udp_echoclient_send(arrSendPulseState , 7);
					}
					if(mygetudpdata.arrUdp_index[3] == 0x0F)
					{//全部参数
						npulseNum = setpar.pulseCountSet;
						ncycle = PreCod.CodCycle;
						unsigned char i = 0;
						arrSendAll[4] = npulseNum;
						arrSendAll[5] = npulseNum >> 8;
						arrSendAll[6] = ncycle;
						arrSendAll[7] = ncycle	>> 8;
						arrSendAll[8] = ncycle	>> 16;
						arrSendAll[9] = ncycle	>> 24;
						arrSendAll[10] = npulseState;							
						udp_echoclient_send(arrSendAll , 13);
					}
					break;		
				}
				break;

				/**	伪随机脉冲频率	**/
				case 4:
//				switch(mygetudpdata.arrUdp_index[2])
//				{
//				/***	装订	***/
//				case 0:
//				//伪随机编码组
//				nrandomGroup = mygetudpdata.arrUdp_index[3];
//				//伪随机编码总数
//				nrandomSum = mygetudpdata.arrUdp_index[4];
//				//伪随机编码当前索引
//				nrandomIndex = mygetudpdata.arrUdp_index[5];
//				//伪随机编码值
//				nrandomData = mygetudpdata.arrUdp_index[6] | mygetudpdata.arrUdp_index[7] >> 8;
//				if(nrandomSum > 2 && nrandomSum <= 2048){
//					//static unsigned short loop = 0;
//					//伪随机数量小于2，那么伪随机就没意义了
//					gRamdomD.arrRandomData[nrandomIndex][0] = nrandomIndex;
//					gRamdomD.arrRandomData[nrandomIndex][1] = nrandomData;
//					//loop++;
//				}
//				break;
//				
//				/***	控制	***/
//				case 1:
//				break;
//				
//				/***	查询	***/
//				case 2:
//				nRrandomSum = nrandomSum;
//				nRrandomGroup = nrandomGroup;
//				//目前只有查询第几组伪随机数据
//				if(nRrandomSum >= 2)
//				{
//					for(unsigned int i = 0 ; i < nRrandomSum ; i++){
//					//读出当前索引
//					//读出周期值
//					//适当延时，否则会卡死上位机
//						nRrandomIndex = gRamdomD.arrRandomData[i][0];
//						nRrandomData =  gRamdomD.arrRandomData[i][1];
//						arrRandomDataReturn[3] =  nRrandomGroup;
//						arrRandomDataReturn[4] = nRrandomSum;
//						arrRandomDataReturn[5] = nRrandomSum >> 8;
//						arrRandomDataReturn[6] = nRrandomIndex;
//						arrRandomDataReturn[7] = nRrandomIndex >> 8;
//						arrRandomDataReturn[8] = nRrandomData;
//						arrRandomDataReturn[9] = nRrandomData >> 8;
//						osDelay(10);					
// 					}
//				}
//				break;

//				}					
				
//				if(mygetudpdata.arrUdp_index[2] == 1)//控制
//				{//控制
//					if(mygetudpdata.arrUdp_index[3] == 1)
//					{
//						ThreadIdTaskSetPWM = osThreadNew(AppTaskSetPWM , NULL , &ThreadSetPWM_Attr);	
//					}
//					else if(mygetudpdata.arrUdp_index[3] == 2)
//						osEventFlagsSet(event_PWMTaskFlag_ID , 0x01U << 0); 
//				}
//				else if(mygetudpdata.arrUdp_index[2] == 0)
//				{//装订
//					
//				}
//				else if(mygetudpdata.arrUdp_index[2] ==2)
//				{//查询
//					
//				}
				break;

				}
			}
		}	
	}
}



