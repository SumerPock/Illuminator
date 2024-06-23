#include "bsp.h"

//void AppTaskIM1266(void *argument)
//{
////	struct UARTState_Machine uartstate_machine = {0,0,0,0,0,0};/*串口状态鸡*/
////	const uint16_t usFrequency = 100; /* 延迟周期 */
////	uint32_t tick;
//	osStatus_t os_Status; 

//	while(1)
//	{
////			os_Status = osMessageQueueGet(msgQueue_lm1266Data, 	/* 消息队列ID */
////																		&GetLm1266Data,     	/* 存放消息的内存 */
////																		NULL,				 					/* 消息优先级 */
////																		osWaitForever);    		/* 超时时间（一直等待） */

//		
////				os_Status = osMessageQueueGet(msgQueue_UdpResData,  /* 消息队列ID */
////										&GetUdpData,          							/* 存放消息的内存 */
////										NULL,						 										/* 消息优先级 */
////										osWaitForever);    									/* 超时时间（一直等待） */

//		
////			if(osOK == os_Status)
////			{
////				for(unsigned short usi = 0; usi < GetLm1266Data.lm1266_real_len ; usi++)
////				{
////					printf("GetLm1266Data.lm1266_resdata[%d] = %d \r\n", usi , GetLm1266Data.lm1266_resdata[usi]);
////				}
////				
////			}	
//	}
//}



