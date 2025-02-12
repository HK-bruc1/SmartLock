#ifndef LCD_H
#define LCD_H

#include "main.h"


#define LCD_SPI_SCL_H 	GPIO_SetBits(GPIOB, GPIO_Pin_13)
#define LCD_SPI_SCL_L 	GPIO_ResetBits(GPIOB, GPIO_Pin_13)
#define LCD_MOSI_H 			GPIO_SetBits(GPIOB, GPIO_Pin_15)
#define LCD_MOSI_L 			GPIO_ResetBits(GPIOB, GPIO_Pin_15)

#define LCD_RES_H 	GPIO_SetBits(GPIOB, GPIO_Pin_10)
#define LCD_RES_L 	GPIO_ResetBits(GPIOB, GPIO_Pin_10)
#define LCD_CS_H	  GPIO_SetBits(GPIOB,GPIO_Pin_12)
#define LCD_CS_L	  GPIO_ResetBits(GPIOB,GPIO_Pin_12)

#define LCD_LED_ON	  GPIO_SetBits(GPIOB,GPIO_Pin_1)
#define LCD_LED_OFF	  GPIO_ResetBits(GPIOB,GPIO_Pin_1)

#define LCD_DATA	  GPIO_SetBits(GPIOB,GPIO_Pin_14)
#define LCD_CMD     GPIO_ResetBits(GPIOB,GPIO_Pin_14)



#define LCD_W  240
#define LCD_H  240

/* 颜色定义开始 */
#define WHITE            0xFFFF 
#define BLACK            0x0000 	  
#define BLUE             0x001F   
#define BRED             0XF81F 
#define GRED 			    0XFFE0 
#define GBLUE			    0X07FF 
#define RED              0xF800 
#define MAGENTA          0xF81F 
#define GREEN            0x07E0 
#define CYAN             0x7FFF 
#define YELLOW           0xFFE0 
#define BROWN 			    0XBC40 //棕色
#define BRRED 			    0XFC07 //棕红色
#define GRAY  			    0X8430 //灰色
#define DARKBLUE      	 0X01CF	//深蓝色
#define LIGHTBLUE      	 0X7D7C	//浅蓝色  
#define GRAYBLUE       	 0X5458 //灰蓝色
#define LIGHTGREEN     	 0X841F //浅绿色
#define LGRAY 			    0XC618 //浅灰色(PANNEL),窗体背景色
#define LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)
/* 颜色定义结束 */



//lcd显示：图片参数结构体用于参数传递
typedef struct _HEADCOLOR
{
   unsigned char scan;
   unsigned char gray;
   unsigned short w;
   unsigned short h;
   unsigned char is565;
   unsigned char rgb;
}HEADCOLOR;




void LCD_clear(u16 color);
void LCD_xy_clear(u16 x,u16 y,u16 w,u16 h,u16 color,u8 mode,u8 *pic);
void LCD_init(void);
void LCD_Ring(u16 x,u16 y,u16 r,u16 colour);
void LCD_shiRing(u16 x,u16 y,u16 r,u16 colour);
void LCD_Point(u16 x,u16 y,u16 color);
void LCD_dis_ch(u16 x,u16 y,u8 ch,u16 color,u8 mode,u16 b_color,u8 size);
void LCD_dis_hz(u16 x,u16 y,u8 *hz,u16 color,u8 mode,u16 b_color,u8 size);
void LCD_dis(u16 x,u16 y,u8 *str,u16 color,u8 mode,u16 b_color,u8 size);
void LCD_dis_pic(u16 x,u16 y,const u8 *pic);
void LCD_dis_number_val(u16 x,u16 y,u8 *numstr,u16 color,u8 mode,u16 b_color);
void LCD_dis_number_pic(u16 x,u16 y,u8 *numstr,u16 color,u8 mode,const u8 *pic);




#endif