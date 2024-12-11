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
  * @brief 	���߲��״̬���Ƕ�����
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
		ethernetif_check_link(&netif);	/*DUBUGģʽ�¿�������ʶ�𣬵��ϵ�󲻽���DUBUGģʽ���޷�ʶ��*/
		if(netif_is_link_up(&netif)) 		/*ֻ�е����߲��Ϻ�Ž���UDP����*/
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
		tick += usFrequency;    /* ����ӳ� */                      
		osDelayUntil(tick);	
	}	
}



/**
  * @brief 	udp���ݽ�����ѯ���Ƕ�����
  * @retval 
  */
void AppTaskLwIPPro(void *argument)
{
	const uint16_t usFrequency = 20; /* �ӳ����� */
	unsigned int tick = 0;
	tick = osKernelGetTickCount(); 	/* ��ȡ��ǰʱ�� */
	osStatus_t os_Status;
	while(1){		
		ethernetif_poll(&netif);
		tick += usFrequency;    			/* ����ӳ� */                      
		osDelayUntil(tick);	
	}
}


/**
  * @brief 
  * @retval udp���ݽ�������ʹ�ö���������
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
	
	/**������Щ�򿪺��������һ���������**/
	/*��ͬ����ͨ����ģʽ-����������*/
	unsigned char arrSendNum[] = {0x08 , 0x02 , 0x02 , 0x01, 0x00 , 0x00 , 0xee , 0xbb};	
	
	/*��ͬ����ͨ����ģʽ-����������*/
	unsigned char arrSendCycle[] = {0x08 , 0x02 , 0x02 , 0x02, 0x00 , 0x00 , 0xee , 0xbb};
	
	/*��ͬ����ͨ����ģʽ-��������״̬*/
	unsigned char arrSendPulseState[] = {0x07 , 0x02 , 0x02 , 0x03, 0x00 , 0xee , 0xbb};	
	
	/*��ͬ����ͨ����ģʽ-����ȫ������*/
	unsigned char arrSendAll[] = { 0x0B , 0x02 , 0x02 , 0x0f , 
																 0x00 , 0x00 , 0x00 , 0x00 , 0x00 ,
																 0xee , 0xbb	};
	
	/*α���ģʽ��ѯ����*/															 
	unsigned char arrRandomDataReturn[] = {0x0b , 0x03 , 0x02 , 0x01 , 
																	 0x00 , 0x00 , 
																	 0x00 , 0x00 , 
																	 0x00 , 0x00 ,
																	 0xEE , 0XBB	};
	
	unsigned int npulseNum = 0;						//��������
	unsigned int ncycle = 0;							//��������
	unsigned int npulseState = 2;	//����״̬

	/**��ȡ**/
	unsigned char  nrandomGroup = 0;				//α���������								 
	unsigned short nrandomSum   = 0;				//α�����������
	unsigned short nrandomIndex	= 0;				//α������뵱ǰ����
	unsigned short nrandomData = 0;					//α�������ֵ
	struct RANDOM	gRamdomD = {0 , 0 , 0, 0, {0 , 0}};	

	/**��ѯ**/
	unsigned char  nRrandomGroup = 0;				//α���������								 
	unsigned short nRrandomSum   = 0;				//α�����������
	unsigned short nRrandomIndex	= 0;				//α������뵱ǰ����
	unsigned short nRrandomData = 0;					//α�������ֵ
																 
	//��ά�������ڴ洢α�������
																 
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
				
				/**	�̶�����Ƶ��	**/
				case 2:
				PreCod.Frequency = mygetudpdata.arrUdp_index[3] | mygetudpdata.arrUdp_index[4] << 8;
				int narr = 0;
				int nRedress = 0;
				switch(mygetudpdata.arrUdp_index[2])
				{
					/***	����װ��	***/
					case 0:	
					setpar.pulseCount = 0;					
					setpar.pulseCountSet = PreCod.Frequency;
					PreCod.CodCycle = mygetudpdata.arrUdp_index[5] | mygetudpdata.arrUdp_index[6] << 8 | mygetudpdata.arrUdp_index[7] << 16;
					if(PreCod.CodCycle >= CYCLELIMITMIN && PreCod.CodCycle <= CYCLELIMITMAX)
						PreCod.CodCycle = PreCod.CodCycle;
					else
						PreCod.CodCycle = CYCLELIMITDEF;
					nRedress = PreCod.CodCycle / 1000;/*��������*/
					/*�Զ�����*/
					narr =  (PreCod.CodCycle * 84 - 1) - arrRedress[nRedress];
					Update_Timer_Arr(narr);
					/*��λ����ֵ*/
//					unsigned short test = mygetudpdata.arrUdp_index[7] | mygetudpdata.arrUdp_index[8] << 8;
//					narr =  (PreCod.CodCycle * 84 - 1) - test;
//					narr =  (PreCod.CodCycle * 84 - 1);
//					Update_Timer_Arr(narr);					
					setpar.nflagGpio = 1;
					break;
					
					/***	�������	***/
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
					
					/***	�����ѯ	***/
					case 2:				
					if(mygetudpdata.arrUdp_index[3] == 0x01)
					{//������		 pulseCountSet				
						npulseNum = setpar.pulseCountSet;
						arrSendNum[4] = npulseNum;
						arrSendNum[5] = npulseNum >> 8;
						unsigned char i = 0;
						udp_echoclient_send(arrSendNum , 8);
					}
					if(mygetudpdata.arrUdp_index[3] == 0x02)
					{//����						
						ncycle = PreCod.CodCycle;
						arrSendCycle[4] = ncycle;
						arrSendCycle[5] = ncycle >> 8;
						arrSendCycle[6] = ncycle >> 16;
						arrSendCycle[7] = ncycle >> 24;
						unsigned char i = 0;
						udp_echoclient_send(arrSendCycle , 10);
					}
					if(mygetudpdata.arrUdp_index[3] == 0x03)
					{//����״̬npulseState
						arrSendPulseState[4] = npulseState;
						unsigned char i = 0;
						udp_echoclient_send(arrSendPulseState , 7);
					}
					if(mygetudpdata.arrUdp_index[3] == 0x0F)
					{//ȫ������
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

				/**	α�������Ƶ��	**/
				case 4:
//				switch(mygetudpdata.arrUdp_index[2])
//				{
//				/***	װ��	***/
//				case 0:
//				//α���������
//				nrandomGroup = mygetudpdata.arrUdp_index[3];
//				//α�����������
//				nrandomSum = mygetudpdata.arrUdp_index[4];
//				//α������뵱ǰ����
//				nrandomIndex = mygetudpdata.arrUdp_index[5];
//				//α�������ֵ
//				nrandomData = mygetudpdata.arrUdp_index[6] | mygetudpdata.arrUdp_index[7] >> 8;
//				if(nrandomSum > 2 && nrandomSum <= 2048){
//					//static unsigned short loop = 0;
//					//α�������С��2����ôα�����û������
//					gRamdomD.arrRandomData[nrandomIndex][0] = nrandomIndex;
//					gRamdomD.arrRandomData[nrandomIndex][1] = nrandomData;
//					//loop++;
//				}
//				break;
//				
//				/***	����	***/
//				case 1:
//				break;
//				
//				/***	��ѯ	***/
//				case 2:
//				nRrandomSum = nrandomSum;
//				nRrandomGroup = nrandomGroup;
//				//Ŀǰֻ�в�ѯ�ڼ���α�������
//				if(nRrandomSum >= 2)
//				{
//					for(unsigned int i = 0 ; i < nRrandomSum ; i++){
//					//������ǰ����
//					//��������ֵ
//					//�ʵ���ʱ������Ῠ����λ��
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
				
//				if(mygetudpdata.arrUdp_index[2] == 1)//����
//				{//����
//					if(mygetudpdata.arrUdp_index[3] == 1)
//					{
//						ThreadIdTaskSetPWM = osThreadNew(AppTaskSetPWM , NULL , &ThreadSetPWM_Attr);	
//					}
//					else if(mygetudpdata.arrUdp_index[3] == 2)
//						osEventFlagsSet(event_PWMTaskFlag_ID , 0x01U << 0); 
//				}
//				else if(mygetudpdata.arrUdp_index[2] == 0)
//				{//װ��
//					
//				}
//				else if(mygetudpdata.arrUdp_index[2] ==2)
//				{//��ѯ
//					
//				}
				break;

				}
			}
		}	
	}
}



