/**
  ******************************************************************************
  * @file    udp_echoserver.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    31-July-2013
  * @brief   UDP echo server
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"
//#include "ip_addr.h"
#include <string.h>
#include <stdio.h>
#include "bsp.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define UDP_SERVER_PORT    80   /* define the UDP local connection port */
#define UDP_CLIENT_PORT    80   /* define the UDP remote connection port */

#define DEST_IP_ADDR0   192
#define DEST_IP_ADDR1   168
#define DEST_IP_ADDR2   0
#define DEST_IP_ADDR3   120

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void udp_echoserver_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const struct ip4_addr *addr, u16_t port);

/* Private functions ---------------------------------------------------------*/

struct udp_pcb *upcb = NULL;

err_t errconnect = {0};
err_t errbind = {0};

ip_addr_t DestIPaddr = {0}; 		//240 : 80
ip_addr_t ServerIPaddr = {0};		//120 : 50

/**
  * @brief  Initialize the server application.
  * @param  None
  * @retval None
  */
void udp_echoserver_init(void)
{
	 IP4_ADDR(&DestIPaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);												//192.168.0.240 80
	 IP4_ADDR(&ServerIPaddr, DEST_IP_ADDR0, DEST_IP_ADDR1, DEST_IP_ADDR2, DEST_IP_ADDR3);	//192.168.0.120	50
   /* Create a new UDP control block  */
   upcb = udp_new();
   if (upcb != NULL)
   {
			/* Bind the upcb to the UDP_PORT port */
			/* Using IP_ADDR_ANY allow the upcb to be used by any local interface */		 
		  /*绑定UDP PCB 为固定ip 240 */
			errbind = udp_bind(upcb , &DestIPaddr , UDP_SERVER_PORT); 
			/*绑定UDP PCB 为固定ip 120*/		 
		  errconnect = udp_connect(upcb , &ServerIPaddr , UDP_CLIENT_PORT); 
      if(errbind == ERR_OK && errconnect == ERR_OK)
      {
        /* Set a receive callback for the upcb */
        udp_recv(upcb, udp_echoserver_receive_callback, NULL);
//				struct pbuf *pq = {0};
				struct pbuf *pq;
				unsigned  char buffer[9] = {0};
				pbuf_take(pq , (unsigned char*)buffer , 9);									//copy data to buf
				udp_send(upcb , pq);	
      }
      else
      {
        udp_remove(upcb);
				//printf("can not bind pcb");
      }
   }
   else
   {
    //printf("can not create pcb");
   } 
}

void udp_echoclient_send(unsigned char DATA[] , unsigned short len)
{
	errbind = udp_bind(upcb , &DestIPaddr , UDP_SERVER_PORT); 				/*绑定UDP PCB 为固定ip 240*/
	errconnect = udp_connect(upcb , &ServerIPaddr , UDP_CLIENT_PORT); /*绑定UDP PCB 为固定ip 120*/
	if(errbind == ERR_OK && errconnect == ERR_OK)
	{
		//struct pbuf *pq = {0};	
		struct pbuf *pq = {0};	
//		if (pq != NULL)
//		{
//			pq = pbuf_alloc(PBUF_TRANSPORT , len , PBUF_POOL);//allocate memory		
//			pbuf_take(pq , (char*)DATA , len);								//copy data to buf	
//			udp_send(upcb , pq);															//send udp data	
//			pbuf_free(pq);
//		}		

			pq = pbuf_alloc(PBUF_TRANSPORT , len , PBUF_POOL);//allocate memory		
			pbuf_take(pq , (char*)DATA , len);								//copy data to buf	
			udp_send(upcb , pq);															//send udp data	
			pbuf_free(pq);
		
	}
	udp_disconnect(upcb); 
}


/**
  * @brief 当端口 UDP_PORT 上收到 UDP 数据组时调用此函数  
  * @param arg 用户提供的参数 (udp_pcb.recv_arg)
  * @param pcb 接收数据的udp_pcb
  * @param p 接收到的数据包缓冲区
  * @param addr 接收数据包的远程 IP 地址
  * @param port 接收数据包的远程端口
  * @retval None
  */
void udp_echoserver_receive_callback(void *arg, 
																		 struct udp_pcb *upcb, 
																		 struct pbuf *p , 
																		 const struct ip4_addr *addr , 
																		 u16_t port)
{
	unsigned char ucGetUdpLen = 0;
	//unsigned char receivebuf[32] = {0};
	//uint8_t send_buf[10] = {0xFF , 0x01 , 0X02 , 0X03 , 0X04 , 0X05 , 0X06 , 0X07 , 0XEE};
//	struct GETUDPDATA SetUdpData;
	struct Get_UdPData getudpdata;
	//unsigned char gerUDPData[32] = {0};
//	int receivelen = 0;
	/*这里面的顺序很重要，打乱会造成UDP发送失败*/
//	struct pbuf *pq;
//	receivelen = p->len;
//	memcpy(receivebuf , p->payload , p->len);
	ucGetUdpLen = p->len;
//	memcpy(&SetUdpData , p->payload , ucGetUdpLen);
	
	memcpy(&getudpdata , p->payload , ucGetUdpLen);
//	SetUdpData.ucGetUdpFlag = 0x01;
	/* Connect to the remote client */
//  udp_connect(upcb , addr , UDP_CLIENT_PORT);
	
//  pq = pbuf_alloc(PBUF_TRANSPORT , 8 , PBUF_POOL);	//allocate memory
//	pbuf_take(pq , (char*)send_buf , 8);							//copy data to buf
	
  //pq = pbuf_alloc(PBUF_TRANSPORT , 32 , PBUF_POOL);	//allocate memory
	//pbuf_take(pq , (char*)SetUdpData.ucGetUdpData , 32);
	
	/*启动消息队列*/
//	osMessageQueuePut(msgQueue_UdpResData , &SetUdpData , NULL , NULL);

	osMessageQueuePut(msgQueue_UdpResData , &getudpdata , NULL , NULL); 
	
//	udp_echoclient_send("this is demo" , 20);		
	
	//event_UdpGetData_ID = osEventFlagsNew(&event_UdpGetData_Attr);

//	osEventFlagsSet(event_UdpGetData_ID , 0x01U << 0);

	/* Tell the client that we have accepted it */
	
	//udp_send(upcb , pq);//send udp data
	
	/* free the UDP connection, so we can accept new clients */
//	udp_disconnect(upcb);
	/* Free the pq buffer */	
//	pbuf_free(pq);  
	/* Free the p buffer */	
  pbuf_free(p);	
}



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
