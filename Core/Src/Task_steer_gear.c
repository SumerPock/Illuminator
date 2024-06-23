#include "bsp.h"

/**
  * @brief  舵机出厂检测模式一，使用堵塞性任务
						读取上位机下发角度，并转发给舵机板卡
  * @retval
  */
void AppTaskSteering_gear(void *argument){
	osStatus_t os_StatusQueue; 
	uint32_t tick;
	tick = osKernelGetTickCount();
  osStatus_t os_Status; 
	BspUart_t getuart1;
	struct COM_Steering_gear send_stee_gear;
	static unsigned short bk = 1;
	unsigned char StreamCRC = 0; 		//编码器CRC校验
	static PUDI_ENCODERFUNC CfgPrm;
	int SpiAngle = 0;
	int PrvSpiAngle = 0; 
	unsigned char StreamStatus = 0;		
	int CountFlag = 0;
	double spiangle = 0;	
	while(1)
	{	
		os_Status = osMessageQueueGet(msgQueue_rxuart1Data , &getuart1 , NULL , osWaitForever);    										
		if(osOK == os_Status){
			if(getuart1.pRxBuf[0] == 0xaa && getuart1.pRxBuf[1] == 0x55 && getuart1.rxLen == 13){
					/* 接收舵机角度存入队列	*/
					if(RING_BUFFER_SUCCESS == (RB_Write_Byte(&stgear_rx, getuart1.pRxBuf[5]))){ 
						#ifdef	DEBUGPR
							SEGGER_RTT_SetTerminal(3);
							SEGGER_RTT_printf(0, "the stger is fifo \r\n");
						#endif	
					}
					if(RING_BUFFER_SUCCESS == (RB_Write_Byte(&stgear_rx, getuart1.pRxBuf[6]))){
						#ifdef	DEBUGPR
							SEGGER_RTT_SetTerminal(3);
							SEGGER_RTT_printf(0, "the stger is fifo >> 8 \r\n");
						#endif	
					}
					/* 读取SPI角度信息存入队列 任务需求 */
					bk = PUDI_ReadAngle(&StreamStatus , &StreamCRC , &SpiAngle);
					if(RING_BUFFER_SUCCESS == (RB_Write_Byte(&stgear_rx, SpiAngle))){
						#ifdef	DEBUGPR
							SEGGER_RTT_SetTerminal(3);
							SEGGER_RTT_printf(0, "the spi data is fifo \r\n");
						#endif
					}
					if(RING_BUFFER_SUCCESS == (RB_Write_Byte(&stgear_rx, SpiAngle >> 8))){
						#ifdef	DEBUGPR
							SEGGER_RTT_SetTerminal(3);
							SEGGER_RTT_printf(0, "the spi data is fifo >> 8 \r\n");
						#endif
					}			
			}	
			
			/**	舵机参数纠正值解包	**/
			else if(getuart1.pRxBuf[0] == 0xCD && getuart1.pRxBuf[1] == 0xAB && getuart1.rxLen == 25){
				unsigned char st_gear_corr[33] = {0};
				
				st_gear_corr[0] = 0xCC;
				st_gear_corr[1] = 0xDD; //头
				st_gear_corr[2] = 0x01;	//序号
				st_gear_corr[3] = 0xD1; //ID
				st_gear_corr[4] = 0x00; //采样率 
				st_gear_corr[5] = 0x00; 
				st_gear_corr[6] = 0x01; //采样数
				st_gear_corr[7] = 0x00;
				st_gear_corr[8]  = getuart1.pRxBuf[0];//头
				st_gear_corr[9]  = getuart1.pRxBuf[1];
				st_gear_corr[10] = getuart1.pRxBuf[2]; //命令类型
				st_gear_corr[11] = getuart1.pRxBuf[3];
				st_gear_corr[12] = getuart1.pRxBuf[4]; //PCB版本号
				st_gear_corr[13] = getuart1.pRxBuf[5]; //软件版本号
				st_gear_corr[14] = getuart1.pRxBuf[6]; //传感器系数
				st_gear_corr[15] = getuart1.pRxBuf[7];
				st_gear_corr[16] = getuart1.pRxBuf[8];
				st_gear_corr[17] = getuart1.pRxBuf[9];
				st_gear_corr[18] = getuart1.pRxBuf[10]; //传感器零位
				st_gear_corr[19] = getuart1.pRxBuf[11];
				st_gear_corr[20] = getuart1.pRxBuf[12]; //电流环增益
				st_gear_corr[21] = getuart1.pRxBuf[13];
				st_gear_corr[22] = getuart1.pRxBuf[14];
				st_gear_corr[23] = getuart1.pRxBuf[15];	
				st_gear_corr[24] = getuart1.pRxBuf[16]; //速度环增益
				st_gear_corr[25] = getuart1.pRxBuf[17];
				st_gear_corr[26] = getuart1.pRxBuf[18];
				st_gear_corr[27] = getuart1.pRxBuf[19];		
				st_gear_corr[28] = getuart1.pRxBuf[20]; //位置环增益
				st_gear_corr[29] = getuart1.pRxBuf[21];
				st_gear_corr[30] = getuart1.pRxBuf[22];
				st_gear_corr[31] = getuart1.pRxBuf[23];					
				st_gear_corr[32] = getuart1.pRxBuf[24]; //校验和				
				udp_echoclient_send(st_gear_corr, 33);
			}
		}		
	}	
}


/**
  * @brief  舵机出厂检测模式二：Hz运动模式
  * @retval
  */
void AppTaskARMSteer_gear(void *argument){
	osStatus_t os_StatusQueue; 
	uint16_t usFrequency = 2;
	uint32_t tick;
	tick = osKernelGetTickCount();
	osStatus_t os_Status; 
	
	short setdata[500] = {0};
	static unsigned short serdatalen = 0;
	static unsigned short sendloop = 0;
	BspUart_t getuart1;
	
	unsigned int uiflag = 0;
	uiflag = osEventFlagsGet(event_HzMode_ID);
	switch(uiflag)
	{
		case 0x00000000:
			break;
		case 0x00000001:
			sendloop = 0;
			out_sine(1 , 3 , &serdatalen , setdata);
			break;
		case 0x00000002:
			sendloop = 0;
			out_sine(1 , 5 , &serdatalen , setdata); 	
			break;
		case 0x00000004:
			sendloop = 0;
			out_sine(1 , 10 , &serdatalen , setdata);
			break;	
		case 0x00000008:
			sendloop = 0;
			out_sine(1 , 15 , &serdatalen , setdata);
			break;	
		case 0x00000010:
			sendloop = 0;
			out_sine(3 , 3 , &serdatalen , setdata);
			break;	
		case 0x00000020:
			sendloop = 0;
			out_sine(3 , 5 , &serdatalen , setdata);
			break;
		case 0x00000040:
			sendloop = 0;
			out_sine(3 , 10 , &serdatalen , setdata);
			break;
		case 0x00000080:
			sendloop = 0;
			out_sine(3 , 15 , &serdatalen , setdata);
			break;
		
		case 0x00000100:
			sendloop = 0;
			out_sine(5 , 3 , &serdatalen , setdata);
			break;
		case 0x00000200:
			sendloop = 0;
			out_sine(5 , 5 , &serdatalen , setdata);
			break;
		case 0x00000400:
			sendloop = 0;
			out_sine(5 , 10 , &serdatalen , setdata);
			break;
		case 0x00000800:
			sendloop = 0;
			out_sine(5 , 15 , &serdatalen , setdata);
			break;

		case 0x00001000:
			sendloop = 0;
			out_sine(7 , 3 , &serdatalen , setdata);
			break;		
		case 0x00002000:
			sendloop = 0;
			out_sine(7 , 5 , &serdatalen , setdata);
			break;		
		case 0x00004000:
			sendloop = 0;
			out_sine(7 , 10 , &serdatalen , setdata);
			break;		
		case 0x00008000:
			sendloop = 0;
			out_sine(7 , 15 , &serdatalen , setdata);
			break;	
		case 0x00010000:
			sendloop = 0;
			out_sine(9 , 3 , &serdatalen , setdata);
			break;		
		case 0x00020000:
			sendloop = 0;
			out_sine(9 , 5 , &serdatalen , setdata);
			break;		
		case 0x00040000:
			sendloop = 0;
			out_sine(9 , 10 , &serdatalen , setdata);
			break;		
		case 0x00080000:
			sendloop = 0;
			out_sine(9 , 15 , &serdatalen , setdata);
			break;		
		
		default:
			break;
	}
	while(1){
		/*
			2024.3.13 测试使用堵塞性任务会造成发送串口数据时间错误，为1.4ms 左右摇摆不定
			2024.3.13 舵机板卡修正参数为2ms，现在以舵机时序为主，修改为堵塞性任务
		*/
		os_Status = osMessageQueueGet(msgQueue_rxuart1Data , &getuart1 , NULL , osWaitForever);
		uiflag = osEventFlagsGet(event_HzMode_ID);
		
		//1.结束任务，
		if(uiflag == 0x100000)	
		{
			int numberint =  RB_Get_Length(&stgear_rxmode2);
			unsigned char tempdata[800] = {0};		
			if(numberint != 0)
			{
				RB_Write_String(&stgear_rxmode2, tempdata, 800);
			}	
			#ifdef	DEBUGPR
						SEGGER_RTT_SetTerminal(3);
						SEGGER_RTT_printf(0,RTT_CTRL_TEXT_BRIGHT_GREEN "thind is stop \r\n");
			#endif
			osThreadExit();
		}
		else
		{//2.
			unsigned char sedbuffer[50] = {0};
			if((sendloop < (serdatalen)) && serdatalen != 0)
			{
				sendloop++;
				*(sedbuffer)= 0xEB;
				*(sedbuffer + 1) = 0x90;
				*(sedbuffer + 2) = 0x08;
				*(sedbuffer + 3) = sendloop;
				*(sedbuffer + 4) = sendloop >> 8;
				*(sedbuffer + 5) = setdata[sendloop];
				*(sedbuffer + 6) = setdata[sendloop] >> 8;
				*(sedbuffer + 7) = *(sedbuffer + 5);
				*(sedbuffer + 8) = *(sedbuffer + 6);
				unsigned short chick = Cal_Sum(sedbuffer , 9) - 0x17b;	
				*(sedbuffer + 9) = chick;
				*(sedbuffer + 10) = chick >> 8;
				*(sedbuffer + 11) = 0x0c;
				BSP_UART_Transmit_DMA(&husart1 , sedbuffer , 12);
				#ifdef	DEBUGPR
					SEGGER_RTT_SetTerminal(3);
					SEGGER_RTT_printf(0 , RTT_CTRL_TEXT_BRIGHT_GREEN "setdata[%d] = %d \r\n" ,sendloop ,setdata[sendloop]);
				#endif			
//				os_Status = osMessageQueueGet(msgQueue_rxuart1Data , &getuart1 , NULL , osWaitForever);
				if(osOK == os_Status)
				{
					if(getuart1.pRxBuf[0] == 0xaa && getuart1.pRxBuf[1] == 0x55 && getuart1.rxLen == 13){	
						if(RING_BUFFER_SUCCESS == (RB_Write_Byte(&stgear_rxmode2, setdata[sendloop]))){
						#ifdef	DEBUGPR
							SEGGER_RTT_SetTerminal(3);
							SEGGER_RTT_printf(0 , RTT_CTRL_TEXT_BRIGHT_GREEN "setdata[sendloop] \r\n");
						#endif
						}
						if(RING_BUFFER_SUCCESS == (RB_Write_Byte(&stgear_rxmode2, setdata[sendloop] >> 8))){
						#ifdef	DEBUGPR
							SEGGER_RTT_SetTerminal(3);
							SEGGER_RTT_printf(0 , RTT_CTRL_TEXT_BRIGHT_GREEN "setdata[sendloop]>>8 \r\n");
						#endif
						}
						if(RING_BUFFER_SUCCESS == (RB_Write_Byte(&stgear_rxmode2, getuart1.pRxBuf[5]))){
						#ifdef	DEBUGPR
							SEGGER_RTT_SetTerminal(3);
							SEGGER_RTT_printf(0 , RTT_CTRL_TEXT_BRIGHT_GREEN "getuart1.pRxBuf[5] \r\n");
						#endif
						}
						if(RING_BUFFER_SUCCESS == (RB_Write_Byte(&stgear_rxmode2, getuart1.pRxBuf[6]))){
						#ifdef	DEBUGPR
							SEGGER_RTT_SetTerminal(3);
							SEGGER_RTT_printf(0 , RTT_CTRL_TEXT_BRIGHT_GREEN "getuart1.pRxBuf[6] \r\n");
						#endif
						}
					}	
				}								
			}
			else
			{
				sendloop = 0;
			}							
		}		

		
	}	
}




