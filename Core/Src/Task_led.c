#include "bsp.h"

void copyArray(unsigned char source[], unsigned char destination[], unsigned char length) {
	memcpy(destination, source, length * sizeof(int));
}



void AppTaskUIMode(void *argument)
{
	const uint16_t usFrequency = 50;	
	unsigned int tick = 0;
	tick = osKernelGetTickCount();
	osStatus_t os_Status;
	unsigned char strct = 0;
	unsigned char nloop = 0;
	unsigned char nFlagUart = 0;
	
	unsigned char nDIsplayFOld = 0;
	unsigned char nDIsplayF = 0;
	unsigned char nDIsplayFMode = 0;
	
	unsigned char ntemp = 0;
	struct preciseCoding PreCod = {0 , 0};
	
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
	unsigned int npulseNum = 0;		//脉冲数量
	unsigned int ncycle = 0;			//脉冲周期
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
	
	
	/**Flag标志**/
	unsigned char nFlagREFCod = 0;			//刷新激光编码值
	unsigned char nFlagREFSWitch = 0;		//刷新激光开关值
	
	while(1)
	{			
		if(nloop <= 10)
		{
			nloop++;
		}else{
			nloop = 0;	
			if(nFlagREFCod == 1)/*激光编码值刷新*/
			{
				nFlagREFCod = 0;
				DISPlay_FunNum(PreCod.CodCycle);
			}
			if(nFlagREFSWitch == 1)/*激光编码开关刷新*/
			{
				nFlagREFSWitch = 0;
				DISPlay_FunLaserSwitch(npulseState);
			}
			/*激光编码时间刷新*/
		}
		
		if(nDIsplayF != nDIsplayFOld)
		{
			switch(ntemp)
			{

				case 1:
				if(nDIsplayFMode == 5)
				{
					DISPlay_Function_2(0);
					DISPlay_Function_3(0);
					DISPlay_Function_4(0);				
					DISPlay_Function_1(1);	
					nDIsplayFMode = 1;
				}						
				break;
				
				case 2:
				if(nDIsplayFMode == 5)
				{
					DISPlay_Function_1(0);
					DISPlay_Function_3(0);
					DISPlay_Function_4(0);
					DISPlay_Function_2(1);
					nDIsplayFMode = 2;		
				}							
				break;
				
				case 3:
				if(nDIsplayFMode == 5)
				{
					DISPlay_Function_1(0);
					DISPlay_Function_2(0);
					DISPlay_Function_4(0);
					DISPlay_Function_3(1);
					nDIsplayFMode = 3;
				}					
				break;
				
				case 4:
				if(nDIsplayFMode == 5)
				{
					DISPlay_Function_1(0);
					DISPlay_Function_2(0);
					DISPlay_Function_3(0);				
					DISPlay_Function_4(1);
					nDIsplayFMode = 4;		
				}					
				break;					

				case 5:
				for(int i = 0 ; i < 5 ; i++)
					LCD_ClearArea(arrLCDClearArea[i][0] , arrLCDClearArea[i][1] , arrLCDClearArea[i][2] , arrLCDClearArea[i][3] , DARKBLUE);				
				if(nDIsplayFOld == 1 || nDIsplayFOld == 2 || nDIsplayFOld == 3 || nDIsplayFOld == 4){
					nDIsplayFMode = 5;
					switch(nDIsplayFOld)
					{
						case 1:
							funFrequencyMode();
							break;
						case 2:
							DisPlayfunrandom();
							break;
						case 3:
							DisPlayfunPCCommand();
							break;
						case 4:
							DisPlayfundebug();
							break;					
					}
				}
				break;
				
				case 6:
					break;
			}				
			nDIsplayFOld = nDIsplayF;
		}
		
		if(g_tBspUsart1.rxFlag == 1){
			g_tBspUsart1.rxFlag = 0;
			if(g_tBspUsart1.pRxBuf[0] == 0xEE){
				nFlagUart++;
			}
			if(g_tBspUsart1.pRxBuf[1] == 0xAA){
				nFlagUart++;
			}
			if(g_tBspUsart1.pRxBuf[2] == 0x01){
				ntemp = g_tBspUsart1.pRxBuf[3];		//UIMode
				nDIsplayF = ntemp;
			}
			else if(g_tBspUsart1.pRxBuf[2] == 0x02)
			{//内同步普通编码模式
				int narr = 0;
				int nRedress = 0;
				if(g_tBspUsart1.pRxBuf[3] == 0x00)
				{//装订指令
					PreCod.Frequency = g_tBspUsart1.pRxBuf[5] << 8 | g_tBspUsart1.pRxBuf[4];					
					PreCod.CodCycle  = g_tBspUsart1.pRxBuf[9] << 24 | g_tBspUsart1.pRxBuf[8] << 16 | 
														 g_tBspUsart1.pRxBuf[7] << 8 | g_tBspUsart1.pRxBuf[6];				
					//通知GUI刷新数据
					setpar.pulseCount = 0;					
//					setpar.pulseCountSet = PreCod.Frequency;
					if(PreCod.Frequency == 0xffff || PreCod.Frequency == 0x00)
						setpar.pulseCountSet = PreCod.Frequency;
					else
						setpar.pulseCountSet = PreCod.Frequency + 1;
					if(PreCod.CodCycle >= CYCLELIMITMIN && PreCod.CodCycle <= CYCLELIMITMAX)
						PreCod.CodCycle = PreCod.CodCycle;
					else
						PreCod.CodCycle = CYCLELIMITDEF;			
					nRedress = PreCod.CodCycle / 1000;/*矫正计算*/
					narr =  (PreCod.CodCycle * 84 - 1) - arrRedress[nRedress];/*自动计算*/
					Update_Timer_Arr(narr);
					nFlagREFCod = 1;
					
					/*上位机传值*/
					//					unsigned short test = mygetudpdata.arrUdp_index[7] | mygetudpdata.arrUdp_index[8] << 8;
					//					narr =  (PreCod.CodCycle * 84 - 1) - test;
					//					narr =  (PreCod.CodCycle * 84 - 1);
					//					Update_Timer_Arr(narr);					
					setpar.nflagGpio = 1;
				}
				
				else if(g_tBspUsart1.pRxBuf[3] == 0x01)
				{//控制指令					
					if(g_tBspUsart1.pRxBuf[4] == 0x01)
					{
						if(PreCod.CodCycle != 0 && setpar.pulseCountSet != 0)
						{
							if(HAL_TIM_Base_Start_IT(&htim2)!= HAL_OK){
							}
							npulseState = 1;
							nFlagREFSWitch = 1;							
						}
					}
					else if(g_tBspUsart1.pRxBuf[4] == 0x02)
					{
						if(PreCod.CodCycle != 0)
						{
							if(HAL_TIM_Base_Stop_IT(&htim2)!= HAL_OK){
							}	
							npulseState = 2;
							nFlagREFSWitch = 1;	
						}			
					}
				}
				
				else if(g_tBspUsart1.pRxBuf[3] == 0x02)
				{//查询指令
					if(g_tBspUsart1.pRxBuf[4] == 0x01)
					{//脉冲数		 pulseCountSet				
						npulseNum = setpar.pulseCountSet;
						arrSendNum[4] = npulseNum;
						arrSendNum[5] = npulseNum >> 8;
						unsigned char i = 0;
						BSP_UART_Transmit_DMA(&husart1,  arrSendNum, 8);
					}
					if(g_tBspUsart1.pRxBuf[4] == 0x02)
					{//周期						
						ncycle = PreCod.CodCycle;
						arrSendCycle[4] = ncycle;
						arrSendCycle[5] = ncycle >> 8;
						arrSendCycle[6] = ncycle >> 16;
						arrSendCycle[7] = ncycle >> 24;
						unsigned char i = 0;
						BSP_UART_Transmit_DMA(&husart1,  arrSendCycle, 10);
					}
					else if(g_tBspUsart1.pRxBuf[4] == 0x03)
					{//脉冲状态npulseState
						arrSendPulseState[4] = npulseState;
						unsigned char i = 0;
						BSP_UART_Transmit_DMA(&husart1,  arrSendPulseState, 7);
					}
					else if(g_tBspUsart1.pRxBuf[4] == 0x0F)
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
						BSP_UART_Transmit_DMA(&husart1,  arrSendAll, 11);						
					}				
				}
				

			}
		}
		
		tick += usFrequency;    /* 相对延迟 */                      
		osDelayUntil(tick);	
	}	
}
