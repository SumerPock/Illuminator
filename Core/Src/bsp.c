/**
  ******************************************************************************
  * @file           : bsp.c
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

TIM_HandleTypeDef htim2;


/**矫正数组 1ms - 120ms**/
unsigned short arrRedress[121] = 
{
8   , 18  , 26  , 34  , 42 , 
54  , 65  , 75  , 85  , 95 ,	
105 , 115 , 125 , 135 , 145 , 
155 , 165 , 175 , 185 , 195 , 
205 , 215 , 225 , 235 , 245 , 
255 , 265 , 275 , 280 , 295 , 
305 , 315 , 325 , 335 , 345 , 
355 , 365 , 375 , 385 , 395 , 
405 , 415 , 425 , 435 , 445 , 
455 , 465 , 475 , 485 , 495 , 
505 , 515 , 525 , 535 , 545 , 
555 , 565 , 575 , 585 , 595 , 
605 , 615 , 625 , 635 , 645 , 
655 , 665 , 675 , 685 , 695 ,
705 , 715 , 725 , 735 , 745 ,
755 , 765 , 775 , 785 , 795 ,
805 , 815 , 825 , 835 , 845 ,
855 , 865 , 875 , 885 , 895 ,
905 , 915 , 925 , 935 , 945 ,
955 , 965 , 975 , 985 , 995 ,
1005, 1015, 1025, 1035, 1045,
1055, 1065, 1075, 1085, 1095,
1105, 1115, 1125, 1135, 1145,
1155, 1165, 1175, 1185, 1195,
1205
};


/*!
* @brief ARM 软重启 
* ETH 重启指令: 0xff 0xaa 0xaa 0xff
* @retval: 无
*/
#define SCB_AIRCR             (*(unsigned long *)0xE000ED0C)      //Reset control Address Register
#define SCB_RESET_VALUE       0x05FA0004                          //reset value ,write to SCB_AIRCR  can reset cpu
void Reset_System(void)
{
	SCB_AIRCR=SCB_RESET_VALUE;
}


void Update_Timer_Arr(uint32_t new_arr_value){
    // 禁止定时器
    __HAL_TIM_DISABLE(&htim2);

    // 更新 ARR 值
    htim2.Instance->ARR = new_arr_value;

    // 生成更新事件
    __HAL_TIM_SET_COUNTER(&htim2, 0); // 可选：重置计数器
    __HAL_TIM_ENABLE(&htim2); // 重新启用定时器
}

void GpioToogleSet(GpioToogleSetPar *rect)
{
	switch (rect->nflagGpio)
	{
	case 1:
		setpar.arrset = setpar.arrtimelow;
//	MX_TIM2_Init();
		if(HAL_TIM_Base_Start_IT(&htim2) != HAL_OK){
		}
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_RESET);		
		//HAL_GPIO_WritePin(GPIOE, GPIO_PIN_10, GPIO_PIN_RESET);
		//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
		rect->nflagGpio = 2;
		#ifdef GPIOTOOGLRDEBUG
			SEGGER_RTT_SetTerminal(1);
			SEGGER_RTT_printf(0, "GpioToogleSet is run rect->nflagGpio = 1 \r\n");	
		#else
		#endif		
		
	break;					

	case 2:
		if(rect->pulseCountSet > 0)//设置脉冲次数
		{
			if(rect->pulseCountSet > rect->pulseCount)//设置脉冲次数 对比 轮询计算数
			{
				rect->pulseCount++;
				#ifdef GPIOTOOGLRDEBUG
					SEGGER_RTT_SetTerminal(1);
					SEGGER_RTT_printf(0, "rect->pulseCount = %d \r\n",rect->pulseCount);	
				#else
				#endif	
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_SET);					
				//HAL_GPIO_WritePin(GPIOE, GPIO_PIN_10, GPIO_PIN_SET);
				//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
				if(HAL_TIM_Base_Stop_IT(&htim2)!= HAL_OK){
				}
				bsp_DelayDWT(675 - 30); //测试发现为5.0325us
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_RESET);		
				//HAL_GPIO_WritePin(GPIOE, GPIO_PIN_10, GPIO_PIN_RESET);
				//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
				if (HAL_TIM_Base_Start_IT(&htim2) != HAL_OK){
				}
			}
			else
			{
				//关闭任务
				if(HAL_TIM_Base_Stop_IT(&htim2)!= HAL_OK){
				}
				setpar.nflagGpio = 0;
			}
		}
		else
		{
		//无限次数出
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_SET);	
			//HAL_GPIO_WritePin(GPIOE, GPIO_PIN_10, GPIO_PIN_SET);
			//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
			if(HAL_TIM_Base_Stop_IT(&htim2)!= HAL_OK){
			}
			bsp_DelayDWT(675 - 30); //测试发现为5.0325us
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_RESET);	
			//HAL_GPIO_WritePin(GPIOE, GPIO_PIN_10, GPIO_PIN_RESET);
			//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
			if (HAL_TIM_Base_Start_IT(&htim2) != HAL_OK){
			}
			#ifdef GPIOTOOGLRDEBUG
			SEGGER_RTT_SetTerminal(2);
			SEGGER_RTT_printf(0, "GpioToogleSet is run rect->nflagGpio = 2 wuxian \r\n");
			#else
			#endif	
		}
	break;
	}
}


/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
//  htim2.Init.Period = 167;
//	htim2.Init.Period = 83;
//	htim2.Init.Period = 419;
//	htim2.Init.Period = setpar.arrset;
	htim2.Init.Period = 839999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}



/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void){
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  /** Configure the main internal regulator output voltage*/
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK){
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK){
    Error_Handler();
  }
}




/* USER CODE BEGIN 4 */

/**
  * @brief  配置软件CS
  * @retval None
  */
void bsp_Spi_Cs_InitGpio(void){
	GPIO_InitTypeDef  GPIO_InitStruct;
	/* 打开GPIO时钟 */
	__HAL_RCC_GPIOB_CLK_ENABLE();
	/* 配置GPIO */
	GPIO_InitStruct.Pin = GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;   		/* 设置推挽输出 */
	GPIO_InitStruct.Pull = GPIO_PULLUP;                 /* 上下拉电阻不使能 */
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  		/* GPIO速度等级 */
	HAL_GPIO_Init(GPIOB , &GPIO_InitStruct);
}


/**
  * @brief  
  * @retval None
  */
unsigned short Cal_Sum(uint8_t * Data_In , uint8_t len){
	unsigned short i = 0 , sum = 0;
	for(i = 0 ; i < len ; i++){
		sum += *(Data_In + i);
	}
	return sum;
}



/**
  * @brief  正选求值
  * @retval None
  */
void out_sine(unsigned char gethz, unsigned char getdu, unsigned short* getnumber, short dataout[500])
{
	uint8_t Data_Hz = gethz, Data_du = getdu;
	uint32_t Useful_Datalen = 0;
	float pi = 3.141592;
	uint16_t i = 0, data_num = 1000 / Data_Hz / 2;		//根据Hz数计算总共需要有多少个数据点，两点间的ms数是2
	for (i = 0; i < data_num; i++)
	{
		dataout[i] = 2 * (0.5 * (sin((2 * pi) / data_num * i) + 1) * Data_du * 500 - Data_du * 250);
	}
	Useful_Datalen = i;
	*getnumber = Useful_Datalen;
}


/**
  * @brief  配置板载LED
  * @retval None
  */
void bsp_LED_InitGpio(void){
	GPIO_InitTypeDef  GPIO_InitStruct;
	/* 打开GPIO时钟 */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	/* 配置GPIO */
	GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;   		/* 设置推挽输出 */
	GPIO_InitStruct.Pull = GPIO_PULLUP;                 /* 上下拉电阻不使能 */
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;  		/* GPIO速度等级 */
	HAL_GPIO_Init(GPIOC , &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;   		/* 设置推挽输出 */
	GPIO_InitStruct.Pull = GPIO_PULLUP;                 /* 上下拉电阻不使能 */
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;  		/* GPIO速度等级 */
	HAL_GPIO_Init(GPIOD , &GPIO_InitStruct);	
	
	GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;   		/* 设置推挽输出 */
	GPIO_InitStruct.Pull = GPIO_PULLUP;                 /* 上下拉电阻不使能 */
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;  		/* GPIO速度等级 */
	HAL_GPIO_Init(GPIOB , &GPIO_InitStruct);		
}


/* USER CODE END 4 */

/*	SPI数据读取	*/
//static PUDI_ENCODERFUNC CfgPrm;
//static int SpiAngle = 0; 
//static int PrvSpiAngle = 0; 
//static unsigned char StreamStatus = 0;		
//static int CountFlag = 0;
//static double spiangle = 0;
/****
	读取SPI传输器数据，及ADC数据
****/
void timer_Periodic_App(void *argument)
{
	
//	static unsigned short loop = 0;
//	unsigned short ad_value = 0;
//	unsigned int ui_value = 0;
//	HAL_ADC_Start(&hadc1);    //启动ADC转换
//	
//	//if(HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK )  //如果转换结果完成
//	ad_value = HAL_ADC_GetValue(&hadc1);	//获取转换值	
//	if(RING_BUFFER_SUCCESS == (RB_Write_Byte(&rb_adc_rx, ad_value))){
//	#ifdef	DEBUGPR
//		SEGGER_RTT_SetTerminal(0);
//		SEGGER_RTT_printf(0, "ad_value is ok \r\n");		
//	#endif
//	}
//	else
//	{
//		#ifdef	DEBUGPR
//			SEGGER_RTT_SetTerminal(0);
//			SEGGER_RTT_printf(0, "ad_value is error \r\n");		
//		#endif
//	}
//	if(RING_BUFFER_SUCCESS == (RB_Write_Byte(&rb_adc_rx, ad_value >> 8))){
//		#ifdef	DEBUGPR
//			SEGGER_RTT_SetTerminal(0);
//			SEGGER_RTT_printf(0, "ad_value >> 8 is ok \r\n");
//		#endif
//	}
//	else
//	{
//		#ifdef	DEBUGPR
//			SEGGER_RTT_SetTerminal(0);
//			SEGGER_RTT_printf(0, "ad_value is error \r\n");			
//		#endif		
//	}
//	
//	
//	unsigned short bk = 1;
//	unsigned char StreamCRC = 0; //编码器CRC校验
//	bk = PUDI_ReadAngle(&StreamStatus , &StreamCRC , &SpiAngle);	
//	if(bk == 0)
//	{
//		#ifdef	DEBUGPR
//			//continue;	//CRC校验错误，重复读取
//			SEGGER_RTT_SetTerminal(0);
//			SEGGER_RTT_printf(0, "crc is fild \r\n");
//		#endif
//	}
//	CountFlag = SpiAngle - PrvSpiAngle;
//	//增加迟滞判断 定义5bit LSB，拟制抖动干扰
//	if(abs(SpiAngle - PrvSpiAngle) > HystLSB){
//		PrvSpiAngle = SpiAngle;
//	}
//	//角度翻转范围为0~65536 或者 65536~0 增加阈值90° 拟制干扰
//	if(CountFlag > 16384){
//		CfgPrm.Counter++;
//	}	
//	else if(-CountFlag > 16384){
//		CfgPrm.Counter--;
//	}	
//	/*此动作由上位机运行*/
//	spiangle = (double)SpiAngle / 65536.0 * 360.0;	
//	//short senddata = (short)(spiangle*100);
//	if(RING_BUFFER_SUCCESS == (RB_Write_Byte(&rb_adc_rx, SpiAngle))){
//		#ifdef	DEBUGPR
//			SEGGER_RTT_SetTerminal(0);
//			SEGGER_RTT_printf(0, "SpiAngle is ok \r\n");
//		#endif
//	}
//	if(RING_BUFFER_SUCCESS == (RB_Write_Byte(&rb_adc_rx, SpiAngle >> 8))){
//		#ifdef	DEBUGPR
//			SEGGER_RTT_SetTerminal(0);
//			SEGGER_RTT_printf(0, "SpiAngle >> 8 is ok \r\n");
//		#endif
//	}
//	#ifdef	DEBUGPR
//		SEGGER_RTT_SetTerminal(0);
//		SEGGER_RTT_printf(0, "timer is runing \r\n");				
//	#endif
}






/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void){
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1){
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */