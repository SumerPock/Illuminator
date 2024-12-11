#ifndef __LCD_H
#define __LCD_H		

//#include "sys.h"


void LCD_Fill(unsigned short xsta,unsigned short ysta,unsigned short xend,unsigned short yend,unsigned short color);//指定区域填充颜色
void LCD_DrawPoint(unsigned short x,unsigned short y,unsigned short color);//在指定位置画一个点
void LCD_DrawLine(unsigned short x1,unsigned short y1,unsigned short x2,unsigned short y2,unsigned short color);//在指定位置画一条线
void LCD_DrawRectangle(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2,unsigned short color);//在指定位置画一个矩形
void Draw_Circle(unsigned short x0,unsigned short y0,unsigned char r,unsigned short color);//在指定位置画一个圆

void LCD_ShowChinese(unsigned short x,unsigned short y,unsigned char *s,unsigned short fc,unsigned short bc,unsigned char sizey,unsigned char mode);//显示汉字串
void LCD_ShowChinese12x12(unsigned short x,unsigned short y,unsigned char *s,unsigned short fc,unsigned short bc,unsigned char sizey,unsigned char mode);//显示单个12x12汉字
void LCD_ShowChinese16x16(unsigned short x,unsigned short y,unsigned char *s,unsigned short fc,unsigned short bc,unsigned char sizey,unsigned char mode);//显示单个16x16汉字
void LCD_ShowChinese24x24(unsigned short x,unsigned short y,unsigned char *s,unsigned short fc,unsigned short bc,unsigned char sizey,unsigned char mode);//显示单个24x24汉字
void LCD_ShowChinese32x32(unsigned short x,unsigned short y,unsigned char *s,unsigned short fc,unsigned short bc,unsigned char sizey,unsigned char mode);//显示单个32x32汉字

void LCD_ShowChar(unsigned short x,unsigned short y,unsigned char num,unsigned short fc,unsigned short bc,unsigned char sizey,unsigned char mode);//显示一个字符
void LCD_ShowString(unsigned short x,unsigned short y,const unsigned char *p,unsigned short fc,unsigned short bc,unsigned char sizey,unsigned char mode);//显示字符串
unsigned int mypow(unsigned char m,unsigned char n);//求幂
void LCD_ShowIntNum(unsigned short x,unsigned short y,unsigned short num,unsigned char len,unsigned short fc,unsigned short bc,unsigned char sizey);//显示整数变量
void LCD_ShowFloatNum1(unsigned short x,unsigned short y,float num,unsigned char len,unsigned short fc,unsigned short bc,unsigned char sizey);//显示两位小数变量
void LCD_ShowFloatNum2(unsigned short x, unsigned short y, float num, unsigned char len, unsigned short fc, unsigned short bc, unsigned char sizey);
void LCD_ShowPicture(unsigned short x,unsigned short y,unsigned short length,unsigned short width, unsigned char pic[]);//显示图片



//画笔颜色
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE           	 0x001F  
#define BRED             0XF81F
#define GRED 			       0XFFE0
#define GBLUE			       0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			     0XBC40 //棕色
#define BRRED 			     0XFC07 //棕红色
#define GRAY  			     0X8430 //灰色
#define DARKBLUE      	 0X01CF	//深蓝色
#define LIGHTBLUE      	 0X7D7C	//浅蓝色  
#define GRAYBLUE       	 0X5458 //灰蓝色
#define LIGHTGREEN     	 0X841F //浅绿色
#define LGRAY 			     0XC618 //浅灰色(PANNEL),窗体背景色
#define LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)


enum ELECTRICITY            /* Defines an enumeration type    */
{
    percentage_90_95 = 240 - 13,       /* Names day and declares a       */
    percentage_85_90 = 240 - 13*2,   		/* variable named workday with    */
    percentage_80_85 = 240 - 13*3,       /* that type                      */
    percentage_75_80 = 240 - 13*4,
    percentage_70_75 = 240 - 13*5,      	/* wednesday is associated with 3 */
    percentage_65_70 = 240 - 13*6,
    percentage_60_65 = 240 - 13*7,
		percentage_55_60 = 240 - 13*8,
		percentage_50_55 = 240 - 13*9,
		percentage_45_50 = 240 - 13*10,
		percentage_40_45 = 240 - 13*11,
		percentage_35_40 = 240 - 13*12,
		percentage_30_35 = 240 - 13*13,
		percentage_25_30 = 240 - 13*14,
		percentage_20_25 = 240 - 13*15,
		percentage_15_20 = 240 - 13*16,
		percentage_10_15 = 240 - 13*17,
		percentage_5_10  = 240 - 13*18
	};
//} electr;




#endif