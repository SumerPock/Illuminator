
#include "lcdfont.h"
#include "bsp.h"



/*
*********************************************************************************************************
*	函 数 名: 
*	功能说明: 在指定区域填充颜色
*	形    参: xsta,ysta   起始坐标 ， xend,yend   终止坐标 ， color       要填充的颜色
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_Fill(unsigned short xsta,unsigned short ysta,unsigned short xend,unsigned short yend,unsigned short color)
{          
	unsigned short color1[1] = {0} , t = 1;
	unsigned int num , num1;
	color1[0] = color;
	num = (xend - xsta) * (yend - ysta);
	LCD_Address_Set(xsta , ysta , xend - 1 , yend - 1);//设置显示范围
	LCD_CS_Clr();
	
//	SPI1->CR1|=1<<11;//设置SPI16位传输模式
//	SPI_Cmd(SPI1, ENABLE);//使能SPI
	
//		while(t)
//		{
//			if(num > 65534)
//			{
//				num -= 65534;
//				num1 = 65534;
//			}
//			else
//			{
//				t=0;
//				num1 = num;
//			}
				
//			if(HAL_SPI_Transmit(&hspi , &color , num1 , 1000) == HAL_OK)
//			{
//				printf("hspi send data error");
//			}
			
//			MYDMA_Config1(DMA1_Channel3,(u32)&SPI1->DR,(u32)color1,num1);
//			SPI_I2S_DMACmd(SPI1,SPI_I2S_DMAReq_Tx,ENABLE);
//			MYDMA_Enable(DMA1_Channel3);
//			while(1)
//			{
//				if(DMA_GetFlagStatus(DMA1_FLAG_TC3)!=RESET)//等待通道4传输完成
//				{
//					DMA_ClearFlag(DMA1_FLAG_TC3);//清除通道3传输完成标志
//					break;
//				}
//			}
			
//				for(unsigned char i = 0 ; i < length ; i++)
//				{
//					for(unsigned char j = 0 ; j < width ; j++)
//					{
//						LCD_WR_DATA8(pic[k*2]);
//						LCD_WR_DATA8(pic[k*2+1]);
//						k++;
//					}
//				}	
			
//		}
		
		unsigned short i , j; 
		for(i = ysta ; i <= yend ; i++)
		{													   	 	
			for(j = xsta ; j <= xend ; j++)
			{
			LCD_WR_DATA(color);//设置光标位置 
			}   
		} 	
		
		LCD_CS_Set();
//		SPI1->CR1=~SPI1->CR1;
//		SPI1->CR1|=1<<11;
//		SPI1->CR1=~SPI1->CR1;//设置SPI8位传输模式
//		SPI_Cmd(SPI1, ENABLE);//使能SPI

}


/*
*********************************************************************************************************
*	函 数 名: 
*	功能说明: 在指定位置画点
*	形    参: x,y 画点坐标 , color 点的颜色
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_DrawPoint(unsigned short x , unsigned short y , unsigned short color)
{
	LCD_Address_Set(x , y , x , y);//设置光标位置 
	LCD_WR_DATA(color);
} 


/*
*********************************************************************************************************
*	函 数 名: 
*	功能说明: 画线
*	形    参: x1,y1起始坐标 , x2,y2终止坐标 , color   线的颜色
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_DrawLine(unsigned short x1,unsigned short y1,unsigned short x2,unsigned short y2,unsigned short color)
{
	unsigned short t; 
	int xerr = 0 , yerr = 0 , delta_x = 0 , delta_y =0 , distance = 0;
	int incx = 0 , incy = 0 , uRow = 0 , uCol = 0;
	delta_x = x2 - x1; //计算坐标增量 
	delta_y = y2 - y1;
	uRow = x1;//画线起点坐标
	uCol = y1;
	if(delta_x > 0)
	{
		incx = 1; //设置单步方向 
	}
	else if (delta_x == 0)
	{
		incx = 0;//垂直线 
	}	
	else
	{
		incx =-1 ; 
		delta_x =-delta_x;
	}
	if(delta_y > 0)
	{
		incy = 1;
	}
		
	else if (delta_y == 0)
	{
		incy = 0;//水平线 
	}
	else 
	{
		incy =-1;
		delta_y =-delta_y;
	}
	if(delta_x > delta_y)
	{
		distance = delta_x; //选取基本增量坐标轴 
	}	
	else 
	{
		distance = delta_y;
	}
	for(t = 0 ; t < distance + 1 ; t++)
	{
		LCD_DrawPoint(uRow , uCol , color);//画点
		xerr+= delta_x;
		yerr+= delta_y;
		if(xerr > distance)
		{
			xerr-= distance;
			uRow+= incx;
		}
		if(yerr > distance)
		{
			yerr-= distance;
			uCol+= incy;
		}
	}
}

/*
*********************************************************************************************************
*	函 数 名: 
*	功能说明: 画矩形
*	形    参: x1,y1起始坐标 , x2,y2终止坐标 , color   线的颜色
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_DrawRectangle(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2,unsigned short color)
{
	LCD_DrawLine(x1,y1,x2,y1,color);
	LCD_DrawLine(x1,y1,x1,y2,color);
	LCD_DrawLine(x1,y2,x2,y2,color);
	LCD_DrawLine(x2,y1,x2,y2,color);
}


/*
*********************************************************************************************************
*	函 数 名: 
*	功能说明: 画圆
*	形    参: x0 , y0,圆心坐标 r,半径 color,圆的颜色
*	返 回 值: 无
* 优 先 级:   
*********************************************************************************************************
*/
void Draw_Circle(unsigned short x0 , unsigned short y0 , unsigned char r , unsigned short color)
{
	int a = 0 , b = 0;
	a = 0 ; b = r;	  
	while( a <= b)
	{
		LCD_DrawPoint(x0 - b , y0 - a , color);             //3           
		LCD_DrawPoint(x0 + b , y0 - a , color);             //0           
		LCD_DrawPoint(x0 - a , y0 + b , color);             //1                
		LCD_DrawPoint(x0 - a , y0 - b , color);             //2             
		LCD_DrawPoint(x0 + b , y0 + a , color);             //4               
		LCD_DrawPoint(x0 + a , y0 - b , color);             //5
		LCD_DrawPoint(x0 + a , y0 + b , color);             //6 
		LCD_DrawPoint(x0 - b , y0 + a , color);             //7
		a++;
		if((a * a + b * b) > (r * r))//判断要画的点是否过远
		{
			b--;
		}
	}
}

/******************************************************************************
      函数说明：显示汉字串
      入口数据：x,y显示坐标
                *s 要显示的汉字串
                fc 字的颜色
                bc 字的背景色
                sizey 字号 可选 16 24 32
                mode:  0非叠加模式  1叠加模式
      返回值：  无
******************************************************************************/
void LCD_ShowChinese(unsigned short x,unsigned short y,unsigned char *s,unsigned short fc,unsigned short bc,unsigned char sizey,unsigned char mode)
{
	while(*s!=0)
	{
		if(sizey==12) 
		{
			LCD_ShowChinese12x12(x,y,s,fc,bc,sizey,mode);
		}	
		else if(sizey==16) 
		{
			LCD_ShowChinese16x16(x,y,s,fc,bc,sizey,mode);
		}
		else if(sizey==24) 
		{
			LCD_ShowChinese24x24(x,y,s,fc,bc,sizey,mode);
		}
		else if(sizey==32) 
		{
			LCD_ShowChinese32x32(x,y,s,fc,bc,sizey,mode);
		}
		else 
		{
			return;
		}	
		s+= 2;
		x+= sizey;
	}
}

/******************************************************************************
      函数说明：显示单个12x12汉字
      入口数据：x,y显示坐标
                *s 要显示的汉字
                fc 字的颜色
                bc 字的背景色
                sizey 字号
                mode:  0非叠加模式  1叠加模式
      返回值：  无
******************************************************************************/
void LCD_ShowChinese12x12(unsigned short x,unsigned short y,unsigned char *s,unsigned short fc,unsigned short bc,unsigned char sizey,unsigned char mode)
{
	unsigned char i = 0 , j = 0 , m = 0;
	unsigned short k = 0 ;
	unsigned short HZnum = 0;//汉字数目
	unsigned short TypefaceNum = 0;//一个字符所占字节大小
	unsigned short x0 = x;
	TypefaceNum = (sizey / 8 + ((sizey % 8) ? 1 : 0)) * sizey;
	                         
	HZnum=sizeof(tfont12)/sizeof(typFNT_GB12);	//统计汉字数目
	for(k=0;k<HZnum;k++) 
	{
		if((tfont12[k].Index[0]==*(s))&&(tfont12[k].Index[1]==*(s+1)))
		{ 	
			LCD_Address_Set(x,y,x+sizey-1,y+sizey-1);
			for(i=0;i<TypefaceNum;i++)
			{
				for(j=0;j<8;j++)
				{	
					if(!mode)//非叠加方式
					{
						if(tfont12[k].Msk[i]&(0x01<<j))LCD_WR_DATA(fc);
						else LCD_WR_DATA(bc);
						m++;
						if(m%sizey==0)
						{
							m=0;
							break;
						}
					}
					else//叠加方式
					{
						if(tfont12[k].Msk[i]&(0x01<<j))	LCD_DrawPoint(x,y,fc);//画一个点
						x++;
						if((x-x0)==sizey)
						{
							x=x0;
							y++;
							break;
						}
					}
				}
			}
		}				  	
		continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
	}
} 


/******************************************************************************
      函数说明：显示单个16x16汉字
      入口数据：x,y显示坐标
                *s 要显示的汉字
                fc 字的颜色
                bc 字的背景色
                sizey 字号
                mode:  0非叠加模式  1叠加模式
      返回值：  无
******************************************************************************/
void LCD_ShowChinese16x16(unsigned short x,unsigned short y,unsigned char *s,unsigned short fc,unsigned short bc,unsigned char sizey,unsigned char mode)
{
	unsigned char i = 0 , j = 0 , m = 0;
	unsigned short k = 0;
	unsigned short HZnum = 0;//汉字数目
	unsigned short TypefaceNum = 0;//一个字符所占字节大小
	unsigned short x0 = x;
  TypefaceNum = (sizey / 8 + ((sizey % 8) ? 1 : 0)) * sizey;
	HZnum = sizeof(tfont16) / sizeof(typFNT_GB16);	//统计汉字数目
	for(k = 0 ; k < HZnum ; k++) 
	{
		if ((tfont16[k].Index[0] ==*(s)) && (tfont16[k].Index[1] ==*(s+1)))
		{ 	
			LCD_Address_Set(x,y,x+sizey-1,y+sizey-1);
			for(i = 0 ; i < TypefaceNum ; i++)
			{
				for(j = 0 ; j < 8 ; j++)
				{	
					if(!mode)//非叠加方式
					{
						if(tfont16[k].Msk[i]&(0x01<<j))
						{
							LCD_WR_DATA(fc);
						}					
						else 
						{
							LCD_WR_DATA(bc);
						}	
						m++;
						if(m % sizey == 0)
						{
							m = 0;
							break;
						}
					}
					else//叠加方式
					{
						if(tfont16[k].Msk[i]&(0x01 << j))
						{
							LCD_DrawPoint(x , y , fc);//画一个点
						}			
						x++;
						if((x-x0)==sizey)
						{
							x = x0;
							y++;
							break;
						}
					}
				}
			}
		}				  	
		continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
	}
} 


/******************************************************************************
      函数说明：显示单个24x24汉字
      入口数据：x,y显示坐标
                *s 要显示的汉字
                fc 字的颜色
                bc 字的背景色
                sizey 字号
                mode:  0非叠加模式  1叠加模式
      返回值：  无
******************************************************************************/
void LCD_ShowChinese24x24(unsigned short x,unsigned short y,unsigned char *s,unsigned short fc,unsigned short bc,unsigned char sizey,unsigned char mode)
{
	unsigned char i = 0 , j = 0 , m = 0;
	unsigned short k = 0;
	unsigned short HZnum = 0;//汉字数目
	unsigned short TypefaceNum = 0;//一个字符所占字节大小
	unsigned short x0 = x;
	TypefaceNum = (sizey / 8 + ((sizey % 8) ? 1 : 0)) * sizey;
	HZnum = sizeof(tfont24) / sizeof(typFNT_GB24);	//统计汉字数目
	for(k = 0 ; k < HZnum ; k++) 
	{
		if ((tfont24[k].Index[0] ==*(s)) && (tfont24[k].Index[1] ==*(s+1)))
		{ 	
			LCD_Address_Set(x , y , x + sizey - 1 , y + sizey - 1);
			for(i = 0 ; i < TypefaceNum ; i++)
			{
				for(j = 0 ; j < 8 ; j++)
				{	
					if(!mode)//非叠加方式
					{
						if(tfont24[k].Msk[i]&(0x01 << j))
							LCD_WR_DATA(fc);
						else
							LCD_WR_DATA(bc);
						m++;
						if((m % sizey) == 0)
						{
							m = 0;
							break;
						}
					}
					else//叠加方式
					{
						if(tfont24[k].Msk[i]&(0x01 << j))
							LCD_DrawPoint(x,y,fc);//画一个点
						x++;
						if((x - x0) == sizey)
						{
							x = x0;
							y++;
							break;
						}
					}
				}
			}
		}				  	
		continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
	}
} 

/******************************************************************************
      函数说明：显示单个32x32汉字
      入口数据：x,y显示坐标
                *s 要显示的汉字
                fc 字的颜色
                bc 字的背景色
                sizey 字号
                mode:  0非叠加模式  1叠加模式
      返回值：  无
******************************************************************************/
void LCD_ShowChinese32x32(unsigned short x,unsigned short y,unsigned char *s,unsigned short fc,unsigned short bc,unsigned char sizey,unsigned char mode)
{
	unsigned char i = 0, j = 0 , m = 0;
	unsigned short k = 0;
	unsigned short HZnum = 0;//汉字数目
	unsigned short TypefaceNum = 0;//一个字符所占字节大小
	unsigned short x0 = x;
	TypefaceNum = (sizey / 8 + ((sizey % 8) ? 1 : 0)) * sizey;
	HZnum = sizeof(tfont32) / sizeof(typFNT_GB32);	//统计汉字数目
	for(k = 0 ; k < HZnum ; k++) 
	{
		if ((tfont32[k].Index[0] ==*(s)) && (tfont32[k].Index[1] ==*(s+1)))
		{ 	
			LCD_Address_Set(x , y , x + sizey - 1 , y + sizey - 1);
			for(i = 0 ; i < TypefaceNum ; i++)
			{
				for(j = 0 ; j < 8 ; j++)
				{	
					if(!mode)//非叠加方式
					{
						if(tfont32[k].Msk[i]&(0x01<<j))
						{
							LCD_WR_DATA(fc);
						}
						else 
						{
							LCD_WR_DATA(bc);
						}
						m++;
						if((m % sizey) == 0)
						{
							m = 0;
							break;
						}
					}
					else//叠加方式
					{
						if(tfont32[k].Msk[i]&(0x01<<j))	
						{
							LCD_DrawPoint(x,y,fc);//画一个点
						}
						x++;
						if((x-x0) == sizey)
						{
							x = x0;
							y++;
							break;
						}
					}
				}
			}
		}				  	
		continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
	}
}


/******************************************************************************
      函数说明：显示单个字符
      入口数据：x,y显示坐标
                num 要显示的字符
                fc 字的颜色
                bc 字的背景色
                sizey 字号
                mode:  0非叠加模式  1叠加模式
      返回值：  无
******************************************************************************/
void LCD_ShowChar(unsigned short x,unsigned short y,unsigned char num,unsigned short fc,unsigned short bc,unsigned char sizey,unsigned char mode)
{
	unsigned char temp = 0 , sizex = 0 , t = 0 , m=0;
	unsigned short i,TypefaceNum;//一个字符所占字节大小
	unsigned short x0 = x;
	sizex = sizey/2;
	TypefaceNum = (sizex / 8 + ((sizex % 8) ? 1 : 0)) * sizey;
	num = num - ' ';    //得到偏移后的值
	LCD_Address_Set(x , y , x + sizex - 1 , y + sizey - 1);  //设置光标位置 
	for(i = 0 ; i < TypefaceNum ; i++)
	{ 
		if(sizey == 12)
		{
			temp = ascii_1206[num][i];		       //调用6x12字体
		}	
		else if(sizey==16)
		{
			temp = ascii_1608[num][i];		 //调用8x16字体
		}	
		else if(sizey==24)
		{
			temp = ascii_2412[num][i];		 //调用12x24字体
		}	
		else if(sizey==32)
		{
			temp = ascii_3216[num][i];		 //调用16x32字体
		}
		else 
		{
			return;
		}
		for(t = 0 ; t < 8 ; t++)
		{
			if(!mode)//非叠加模式
			{
				if(temp&(0x01 << t))
				{
					LCD_WR_DATA(fc);
				}
				else
				{
					LCD_WR_DATA(bc);
				}
				m++;
				if((m % sizex) == 0)
				{
					m = 0;
					break;
				}
			}
			else//叠加模式
			{
				if(temp&(0x01 << t))
				{
					LCD_DrawPoint(x , y , fc);//画一个点
				}
				x++;
				if((x - x0) == sizex)
				{
					x = x0;
					y++;
					break;
				}
			}
		}
	}   	 	  
}


/******************************************************************************
      函数说明：显示字符串
      入口数据：x,y显示坐标
                *p 要显示的字符串
                fc 字的颜色
                bc 字的背景色
                sizey 字号
                mode:  0非叠加模式  1叠加模式
      返回值：  无
******************************************************************************/
void LCD_ShowString(unsigned short x , unsigned short y , const unsigned char *p , unsigned short fc , unsigned short bc , unsigned char sizey , unsigned char mode)
{         
	while(*p!='\0')
	{       
		LCD_ShowChar(x , y , *p , fc , bc , sizey , mode);
		x+= sizey/2;
		p++;
	}  
}


/******************************************************************************
      函数说明：显示数字
      入口数据：m底数，n指数
      返回值：  无
******************************************************************************/
unsigned int mypow(unsigned char m,unsigned char n)
{
	unsigned int result = 1;	 
	while(n--)
	{
		result*= m;
	}
	return result;
}


/******************************************************************************
      函数说明：显示整数变量
      入口数据：x,y显示坐标
                num 要显示整数变量
                len 要显示的位数
                fc 字的颜色
                bc 字的背景色
                sizey 字号
      返回值：  无
******************************************************************************/
void LCD_ShowIntNum(unsigned short x,unsigned short y,unsigned short num,unsigned char len,unsigned short fc,unsigned short bc,unsigned char sizey)
{         	
	unsigned char t = 0 , temp = 0;
	unsigned char enshow = 0;
	unsigned char sizex = sizey / 2;
	for(t = 0 ; t < len ; t++)
	{
		temp = (num / mypow(10 , len - t - 1)) % 10;
		if((enshow == 0) && (t < (len -1 )))
		{
			if(temp == 0)
			{
				LCD_ShowChar(x + t *sizex , y , ' ' , fc , bc , sizey , 0);
				continue;
			}
			else
			{
				enshow = 1; 
			}
			
		 	 
		}
	 	LCD_ShowChar(x + t * sizex , y , temp + 48 , fc , bc , sizey , 0);
	}
} 



/******************************************************************************
      函数说明：显示两位小数变量
      入口数据：x,y显示坐标
                num 要显示小数变量
                len 要显示的位数
                fc 字的颜色
                bc 字的背景色
                sizey 字号
      返回值：  无
******************************************************************************/
void LCD_ShowFloatNum1(unsigned short x,unsigned short y,float num,unsigned char len,unsigned short fc,unsigned short bc,unsigned char sizey)
{         	
	unsigned char t = 0 , temp = 0 , sizex = 0;
	unsigned short num1 = 0;
	sizex = sizey / 2;
	num1 = num * 100;
	for(t = 0 ; t < len ; t++)
	{
		temp=(num1 / mypow(10 , len - t - 1)) % 10;
		if(t == (len - 2))
		{
			LCD_ShowChar(x + (len - 2) * sizex , y , '.' , fc , bc , sizey , 0);
			t++;
			len+= 1;
		}
	 	LCD_ShowChar(x + t * sizex , y , temp + 48 , fc , bc , sizey , 0);
	}
}

/******************************************************************************
      函数说明：显示三位小数变量
      入口数据：x,y显示坐标
                num 要显示小数变量
                len 要显示的位数（含小数点和小数位）
                fc 字的颜色
                bc 字的背景色
                sizey 字号
      返回值：  无
******************************************************************************/
void LCD_ShowFloatNum2(unsigned short x, unsigned short y, float num, unsigned char len, unsigned short fc, unsigned short bc, unsigned char sizey)
{	
	unsigned char t = 0, temp = 0, sizex = 0;
	unsigned long num1 = 0;  // 使用 unsigned long 以避免溢出
	sizex = sizey / 2;

	// 将浮点数转换为整数，保留三位小数
	num1 = (unsigned long)(num * 1000.0f + 0.5f);  // 加上0.5是为了四舍五入

	// 计算整数部分的位数
	unsigned long integerPart = (unsigned long)num;
	unsigned char integerDigits = 0;
	do {
			integerDigits++;
			integerPart /= 10;
	} while (integerPart > 0);

	for (t = 0; t < len; t++)
	{
			if (t == integerDigits)  // 插入小数点
			{
					LCD_ShowChar(x + t * sizex, y, '.', fc, bc, sizey, 0);
					t++;
					len += 1;
			}
			temp = (num1 / mypow(10, len - t - 1)) % 10;
			LCD_ShowChar(x + t * sizex, y, temp + '0', fc, bc, sizey, 0);
	}
}
/******************************************************************************
      函数说明：显示图片
      入口数据：x,y起点坐标
                length 图片长度
                width  图片宽度
                pic[]  图片数组    
      返回值：  无
******************************************************************************/
void LCD_ShowPicture(unsigned short x,unsigned short y,unsigned short length,unsigned short width, unsigned char pic[])
{
	unsigned char t = 1;
	unsigned int k=0;
	unsigned int num = length * width * 2 , num1;
	LCD_Address_Set(x , y , x + length - 1 , y + width - 1);
	LCD_CS_Clr();
//	while(t)
//	{
//	  if(num > 65534)
//		{
//			num-= 65534;
//			num1= 65534;
//		}
//		else
//		{
//			t = 0;
//			num1 = num;
//		}
////		MYDMA_Config(DMA1_Channel3,(u32)&SPI1->DR,(u32)pic,num1);
////		SPI_I2S_DMACmd(SPI1,SPI_I2S_DMAReq_Tx,ENABLE);
////		MYDMA_Enable(DMA1_Channel3);
////		while(1)
////		{
////			if(DMA_GetFlagStatus(DMA1_FLAG_TC3)!=RESET)//等待通道4传输完成
////			{
////				DMA_ClearFlag(DMA1_FLAG_TC3);//清除通道4传输完成标志
////				break; 
////			}
////		}
//			
//		pic+= 65534;
//	}

		for(unsigned char i = 0 ; i < length ; i++)
		{
			for(unsigned char j = 0 ; j < width ; j++)
			{
				LCD_WR_DATA8(pic[k*2]);
				LCD_WR_DATA8(pic[k*2+1]);
				k++;
			}
		}	
	LCD_CS_Set();
}

