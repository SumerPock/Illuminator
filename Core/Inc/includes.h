/*
*********************************************************************************************************
*
*	模块名称 : 头文件汇总
*	文件名称 : includes.h
*	版    本 : V1.0
*	说    明 : 当前使用头文件汇总
*
*	修改记录 :
*		版本号    日期        作者     说明
*		V1.0    2019-02-16  Eric2013   首次发布
*
*	Copyright (C), 2019-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef  __INCLUDES_H__
#define  __INCLUDES_H__

/*
*********************************************************************************************************
*                                         标准库
*********************************************************************************************************
*/
#include  <stdarg.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <math.h>


/*
*********************************************************************************************************
*                                           宏定义
*********************************************************************************************************
*/
/* 给TCPnet任务发的事件标志 */
#define KEY1_BIT0	(1 << 0)
#define KEY2_BIT1	(1 << 1)
#define KEY3_BIT2   (1 << 2)


/*
*********************************************************************************************************
*                                         其它库
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           OS和系统库
*********************************************************************************************************
*/
#include "cmsis_os2.h"

#include "lwip/dhcp.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/priv/tcpip_priv.h"
#include "lwip/priv/tcp_priv.h"
#include "netif/etharp.h"
#include "ethernetif.h"
#include "lwip/sys.h"
#include "lwip/api.h"

#include "lwip/opt.h"

/*
*********************************************************************************************************
*                                           宏定义
*********************************************************************************************************
*/



/*
*********************************************************************************************************
*                                        APP / BSP
*********************************************************************************************************
*/
#include "bsp.h"



/*
*********************************************************************************************************
*                                          变量和函数
*********************************************************************************************************
*/
/* 方便RTOS里面使用 */
extern void SysTick_ISR(void);
extern void TCPnetTest(void);
extern void LwIPTest(void);
extern unsigned char net_init(void);

extern volatile uint8_t g_ucEthLinkStatus;
extern struct netif netif;


#define bsp_ProPer1ms  SysTick_ISR

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
