#include "bsp.h"


void AppTaskSetPWM(void *argument)
{	
	osStatus_t os_Status;
	osStatus_t os_StatusQueue;
	struct preciseCoding PreCod = {0 , 0};
	/*����ȡ��*/
	os_Status = osMessageQueueGet(msgQueue_PreFreData , &PreCod , NULL , 0);   	
	//GetBaseCoding.baseCodFrequency
	
	uint16_t usFrequency = 1;
	uint32_t tick;
	tick = osKernelGetTickCount();
//	setpar.nflagGpio = 1;				// 	 1 ����״̬����ʼ
//	setpar.pulseCountSet = -1;  // 	-1 �������޴�
//	setpar.pulseCountSet = PreCod.Frequency;
//	setpar.arrtimelow = (PreCod.CodCycle * 10) * 84 -1;
	
//	Update_Timer_Arr(setpar.arrtimelow);
	
//	printf("PreCod.CodCycle = %d \r\n" , PreCod.CodCycle);
//	printf("setpar.arrtimelow = %d \r\n" , setpar.arrtimelow);
//	GpioToogleSet(&setpar);			//	��ִ��һ�鿪��
//	event_Time2Flag_ID	= osEventFlagsNew(&event_Time2Flag_Attr);	
//	unsigned char onoff = 0;
//	printf("the ThreadIdTaskSetPWM on is ok\r\n");
//	timerID_Periodic = osTimerNew(timer_Periodic_App , osTimerPeriodic , NULL , &timer_Periodic_Attr);	
//	
//	
//	GpioToogleSetPar setpar = {839999, 2519999, 0, 0, 0, 0};
	
	while(1)
	{
//		setpar.nflagtime2 = osEventFlagsGet(event_Time2Flag_ID);	
//		if(setpar.nflagtime2 == 1)
//		{
//			osEventFlagsClear(event_Time2Flag_ID , 0x00000001U);
//			GpioToogleSet(&setpar);
//			setpar.nflagtime2 = 0;				
//		}else
//		{
//			onoff = osEventFlagsGet(event_PWMTaskFlag_ID);						/*�����»��ǿ�ϵͳ��־λ�����ƴ�����ĹرձȽϺ���*/
//			if(onoff == 1)
//			{
//				onoff = 0;
//				setpar.arrtimelow = 0;
//				osEventFlagsClear(event_PWMTaskFlag_ID, 0x01U << 0);
//				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_10, GPIO_PIN_RESET);
//				SEGGER_RTT_SetTerminal(4);
//				SEGGER_RTT_printf(0, "the gpio is off \r\n");	
//				osThreadExit();		
//			}
//			else
//			{
//			}			
//		}		
	


		tick += usFrequency;                          
		osDelayUntil(tick);		
	}
}