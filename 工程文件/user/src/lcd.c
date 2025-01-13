#include "main.h"


/**************************************************
*函数名    ：LCD_spi_IO_init
*函数功能  ：模拟SPI的管脚初始函数
*函数参数  ：无
*函数返回值：无
*函数描述  ：LCD_CLK----------------------------PB13     
            LCD_SDA----------------------------PB15        
****************************************************/
void LCD_spi_IO_init(void)
{
	//GPIO初始化
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//1.开时钟
	
	//推挽输出
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin =  GPIO_Pin_13 | GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Fast_Speed;
	GPIO_Init(GPIOB, &GPIO_InitStruct);	
	
	//空闲
	LCD_SPI_SCL_L;
}



/**************************************************
*函数名    ：LCD_spi_byte
*函数功能  ：SPI传输一个字节函数
*函数参数  ：u8 data
*函数返回值：无
*函数描述  ：    
****************************************************/
void LCD_spi_byte(u8 data)
{
	u8 i;

	/*传输数据*/
	for(i=0;i<8;i++)
	{
		LCD_SPI_SCL_L;        //拉低时钟线，主机可以写数据   					
		
		if(data & 0x80) LCD_MOSI_H; 
		else LCD_MOSI_L;
		
		LCD_SPI_SCL_H;       //产生上升沿，帮助从机，
		
		data = data << 1;     //发送下一为数据
	}	

}



/**************************************************
*函数名    ：LCD_IO_init
*函数功能  ：LCD的管脚初始函数
*函数参数  ：无
*函数返回值：无
*函数描述  ： LEDK-----PB1        背光灯引脚        //通用输出
            RESET----PB10        复位引脚          //通用输出
            LCD_CS---PB12        片选引脚          //通用输出	
            LCD_D/C--PB14        数据命令选择线    //通用输出		  
****************************************************/
void LCD_IO_init(void)
{
	//GPIO初始化
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//1.开时钟
	
	//推挽输出
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin =  GPIO_Pin_1 | GPIO_Pin_10 | GPIO_Pin_12 | GPIO_Pin_14;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStruct);	
	
	//初始状态
	LCD_LED_OFF;               //背光关
	LCD_CS_H;                  //片选拉高
}


/**************************************************
*函数名    ：st7789vm_rest
*函数功能  ：对LCD复位函数
*函数参数  ：无
*函数返回值：无
*函数描述  ：  
****************************************************/
void st7789vm_rest(void)
{
	LCD_RES_L;
	tim5Delay_Ms(100);
	LCD_RES_H;
	
}


/**************************************************
*函数名    ：st7789vm_write_command
*函数功能  ：对LCD写命令函数
*函数参数  ：u8 cmd
*函数返回值：无
*函数描述  ：    
****************************************************/
void st7789vm_write_command(u8 cmd)
{
	LCD_CS_L;//先拉低片选
	LCD_CMD;//表示发送的是命令
	LCD_spi_byte(cmd);//具体发送的8位 命令
	LCD_CS_H;//拉高片选
}



/**************************************************
*函数名    ：st7789vm_write_command_parameter
*函数功能  ：对LCD写命令参数值函数
*函数参数  ：u8 cmd
*函数返回值：无
*函数描述  ：    
****************************************************/
void st7789vm_write_command_parameter(u8 cmd_data)
{
	LCD_CS_L;//先拉低片选
	LCD_DATA;//表示发送的是数据
	LCD_spi_byte(cmd_data);//具体发送的8位 命令参数
	
	LCD_CS_H;//拉高片选
}



/**************************************************
*函数名    ：st7789vm_write_data
*函数功能  ：对LCD写数据函数
*函数参数  ：u16 data
*函数返回值：无
*函数描述  ：坐标值，颜色值   
****************************************************/
void st7789vm_write_data(u16 data)
{
	LCD_CS_L;//先拉低片选
	LCD_DATA;//表示发送的是数据
	LCD_spi_byte(data>>8);//具体发送的8位 命令参数,高八位再发低八位
	LCD_spi_byte(data);
	LCD_CS_H;//拉高片选
}



/**************************************************
*函数名    ：LCD_init
*函数功能  ：对LCD初始函数
*函数参数  ：无
*函数返回值：无
*函数描述  ：  
****************************************************/
void LCD_init(void)
{
	//SPI_IO初始化
	LCD_spi_IO_init();
	//LCD_IO初始化
	LCD_IO_init();
	//LCD复位
	st7789vm_rest();
	
	//IC驱动代码移植
	/* start initial sequence */ 
	st7789vm_write_command(0x36);   
	st7789vm_write_command_parameter(0x00);

	st7789vm_write_command(0x3A); 
	st7789vm_write_command_parameter(0x05);

	st7789vm_write_command(0xB2);
	st7789vm_write_command_parameter(0x0C);
	st7789vm_write_command_parameter(0x0C);
	st7789vm_write_command_parameter(0x00);
	st7789vm_write_command_parameter(0x33);
	st7789vm_write_command_parameter(0x33); 

	st7789vm_write_command(0xB7); 
	st7789vm_write_command_parameter(0x35);  

	st7789vm_write_command(0xBB);
	st7789vm_write_command_parameter(0x19);

	st7789vm_write_command(0xC0);
	st7789vm_write_command_parameter(0x2C);

	st7789vm_write_command(0xC2);
	st7789vm_write_command_parameter(0x01);

	st7789vm_write_command(0xC3);
	st7789vm_write_command_parameter(0x12);   

	st7789vm_write_command(0xC4);
	st7789vm_write_command_parameter(0x20);  

	st7789vm_write_command(0xC6); 
	st7789vm_write_command_parameter(0x0F);    

	st7789vm_write_command(0xD0); 
	st7789vm_write_command_parameter(0xA4);
	st7789vm_write_command_parameter(0xA1);

	st7789vm_write_command(0xE0);
	st7789vm_write_command_parameter(0xD0);
	st7789vm_write_command_parameter(0x04);
	st7789vm_write_command_parameter(0x0D);
	st7789vm_write_command_parameter(0x11);
	st7789vm_write_command_parameter(0x13);
	st7789vm_write_command_parameter(0x2B);
	st7789vm_write_command_parameter(0x3F);
	st7789vm_write_command_parameter(0x54);
	st7789vm_write_command_parameter(0x4C);
	st7789vm_write_command_parameter(0x18);
	st7789vm_write_command_parameter(0x0D);
	st7789vm_write_command_parameter(0x0B);
	st7789vm_write_command_parameter(0x1F);
	st7789vm_write_command_parameter(0x23);

	st7789vm_write_command(0xE1);
	st7789vm_write_command_parameter(0xD0);
	st7789vm_write_command_parameter(0x04);
	st7789vm_write_command_parameter(0x0C);
	st7789vm_write_command_parameter(0x11);
	st7789vm_write_command_parameter(0x13);
	st7789vm_write_command_parameter(0x2C);
	st7789vm_write_command_parameter(0x3F);
	st7789vm_write_command_parameter(0x44);
	st7789vm_write_command_parameter(0x51);
	st7789vm_write_command_parameter(0x2F);
	st7789vm_write_command_parameter(0x1F);
	st7789vm_write_command_parameter(0x1F);
	st7789vm_write_command_parameter(0x20);
	st7789vm_write_command_parameter(0x23);

	st7789vm_write_command(0x21); 
	st7789vm_write_command(0x11); 
	st7789vm_write_command(0x29); 
	
	//开背光
	LCD_LED_ON;
	//清屏
    LCD_clear(BLACK);
}




/**************************************************
*函数名    ：LCD_clear
*函数功能  ：清屏
*函数参数  ：u16 color
*函数返回值：无
*函数描述  ：两个点坐标可以确定一个矩形区域的位置，再把区域内的颜色值全部填充为同一个颜色值，就相当于把区域内的颜色全部清除了
****************************************************/
void LCD_clear(u16 color)
{
	/*确定区域*/
		//确定x方向，从0~239，240个像素点没错误
		st7789vm_write_command(0x2A);     //横坐标命令
		//起始横坐标
		st7789vm_write_data(0);
		//结束横坐标，映射成坐标值
		st7789vm_write_data(LCD_W-1);
		
		//确定Y方向
		st7789vm_write_command(0x2B);     //纵坐标命令
		//起始纵坐标
		st7789vm_write_data(0);
		//结束纵坐标
		st7789vm_write_data(LCD_H-1);
	
	
	/*确定颜色*/
	//屏幕是二维矩阵形式，但是在驱动控制器中对应位置的颜色信息是一维数组形式线性存储的，所以线性循环即可填充
	st7789vm_write_command(0x2C);     //颜色命令
	//这算的是像素点的个数，也没有错，240*240
	for(u32 i=0;i<LCD_W*LCD_H;i++)
	{
		st7789vm_write_data(color);
	}
}


//指定区域清屏
void LCD_xy_clear(u16 x_start,u16 y_start,u16 x_end,u16 y_end,u16 color){
	/*确定区域*/
		//确定x方向
		st7789vm_write_command(0x2A);     //横坐标命令
		//起始横坐标
		st7789vm_write_data(x_start);
		//结束横坐标，映射成坐标值，从0开始。比如你想划分的区域为x:1~120,y:1~120的位置，主观120的位置在119的坐标上
		//x和y轴的范围在0~239，所以x_end-1,后面算总的像素点个数时，两个极端位置都是算一个像素点的
		st7789vm_write_data(x_end-1);
		
		//确定Y方向
		st7789vm_write_command(0x2B);     //纵坐标命令
		//起始纵坐标
		st7789vm_write_data(y_start);
		//结束纵坐标
		st7789vm_write_data(y_end-1);
	
	
	/*确定颜色*/
	//屏幕是二维矩阵形式，但是在驱动控制器中对应位置的颜色信息是一维数组形式线性存储的，所以线性循环即可填充
	//根据设置的区域大小，循环填充颜色
	//设置了区域，多出的颜色值也无效，u32 i=0;i<LCD_W*LCD_H;i++
	//到了设置的边界会自动换行，次数溢出也不会影响填充的位置
	st7789vm_write_command(0x2C);     //颜色命令
	//计算需要填充多少次，那就要计算总计有多少个像素点了，x方向有多少个像素点，y方向有多少个像素点
	//如1~120，0~119，实际上120个像素点，所以加1
	//两个极端位置都是算一个像素点的
	for(u32 i=0;i<(x_end-x_start+1)*(y_end-y_start+1);i++)
	{
		st7789vm_write_data(color);
	}

}



/**************************************************
*函数名    ：LCD_Point
*函数功能  ：打点函数
*函数参数  ：u16 x,u16 y,u16 color
*函数返回值：无
*函数描述  ：  精准到一个像素点位置上，也就是x的开始和结束都是相同的值，y的开始和结束也是相同的值
****************************************************/
void LCD_Point(u16 x,u16 y,u16 color)
{
	/*确定区域*/
	//确定x方向
	st7789vm_write_command(0x2A);     //横坐标命令
	//起始横坐标
	 st7789vm_write_data(x);
	//结束横坐标
	 st7789vm_write_data(x);
	
	//确定Y方向
	st7789vm_write_command(0x2B);     //纵坐标命令
	//起始纵坐标
	st7789vm_write_data(y);
	//结束纵坐标
	st7789vm_write_data(y);
	
	
	/*确定颜色*/
	st7789vm_write_command(0x2C);     //颜色命令
	st7789vm_write_data(color);

}



/******************************************************************************
*函数名    ：LCD_dis_ch
*函数功能  ：LCD屏幕显示可设定大小的字符
*函数参数  ：u16 x,u16 y,u8 ch,u16 color,u8 size,u8 mode,u16 b_color
*函数返回值：无
*函数描述  ：mode 传入0   无背景色  背景色参数随便传
            mode 传入1   有背景色  背景色参数根据需求设定
			size  传入16  16*16
			size  传入32  32*32
********************************************************************************/
void LCD_dis_ch(u16 x,u16 y,u8 ch,u16 color,u8 mode,u16 b_color,u8 size){
	u8 n,i,j;
	u16 temp;
	//计算字符与空格字符的偏移量：只存放了空格字符开始的字符表
	n = ch - ' ';
	//字符显示空间：行数显示需要32行，每一行16个像素点需要根据两个模数据元素来判断是否打点
	//模数据是两个字节数据表示一行是否需要打点
	for(i=0;i<size;i++){
		//取出一行像素点的模数据
		if(size==16){
			//偏移一个字符，相当于模数据数组元素偏移16个元素，16*16，实际上字宽是8，高是16
			temp = ASC16[n*16+i];
		}else if(size==32){
			//偏移一个字符，相当于模数据数组元素偏移64个元素。32*32,字宽是16，高是32
			temp = ASC32[n*64+(2*i)]<<8 | ASC32[n*64+2*i+1];
		}

		//每行的像素点打点检测，具体看每一行的像素点有多少个像素点，从高位到低位判断
		for(j=0;j<(size/2);j++){
			if(temp & (1<<(size/2-1-j))){
				LCD_Point(x+j,y+i,color);//字体颜色
			}
			else{
				if(mode == 1){
					//不是目标点就打背景色，实现覆盖旧的颜色数据,不需要清屏，实现局部刷新
					LCD_Point(x+j,y+i,b_color);//背景色
				}
			}
		}
	}
}



/******************************************************************************
*函数名    ：LCD_dis_hz
*函数功能  ：LCD屏幕显示可设定大小的字符
*函数参数  ：u16 x,u16 y,u8 *hz,u16 color,u8 size,u8 mode,u16 b_color
*函数返回值：无
*函数描述  ：mode 传入0   无背景色  背景色参数随便传
			mode 传入1   有背景色  背景色参数根据需求设定
			size  传入16  16*16
			size  传入32  32*32
			汉字需要两个字节，相当于传字符串,给一个地址。
********************************************************************************/
void LCD_dis_hz(u16 x,u16 y,u8 *hz,u16 color,u8 mode,u16 b_color,u8 size){
    u8 n = 0;
	u8 i,j;
    u32 temp;
    
    // 计算要显示的汉字与第一个汉字的偏移量,n为汉字编号不是数组元素下标
	//计算模数据的开始位置
    while(HZ[n*2] != '\0'){
		// 比较传入的汉字与当前汉字是否相等,如果相等就找到了，跳出循环。用汉字地址取值，比较前后半部分的字符码
		//地址取值也是对应的字符编码
		if(*hz == HZ[2*n]  && *(hz+1) == HZ[2*n+1]){
			break;
		}
		n++;
	}
    //没有找到的话，就不显示直接返回
    if(HZ[2*n] == '\0'){
        return;
    }
    
    //根据模数据判断点阵是否打点
	for(i=0;i<size;i++){
		// 16*16点阵的显示
		if(size==16){
			// 每行数据由两个字节模数据组成是否打点，实际高宽都是16*16，所以模数据数组元素偏移32个元素
			//每一行像素点判断时，取16位也就是两个u8元素
			//第一个汉字的模数据为0，1下标元素，第二个汉字的模数据为2，3下标元素，以此类推
			temp = (HZ16[n*32+i*2]<<8) | HZ16[n*32+i*2+1];
		}
		// 32*32点阵的显示
		if(size==32){
			// 每行数据由四个字节模数据组成是否打点
			//第一个汉字的模数据为0，1，2，3下标元素，第二个汉字的模数据为4，5，6，7下标元素，以此类推
            temp = (HZ32[n*128+i*4]<<24) | (HZ32[n*128+i*4+1]<<16) | (HZ32[n*128+i*4+2]<<8) | (HZ32[n*128+i*4+3]);
		}
		
		//每一行逐个像素点判断
		for(j=0;j<size;j++){
			if(temp & (1<<(size-1-j))){  // 从高位开始判断
				LCD_Point(x+j,y+i,color);
			}
			else if(mode == 1){
				LCD_Point(x+j,y+i,b_color);
			}
		}
	}

	return;
}



/******************************************************************************
*函数名    ：LCD_dis
*函数功能  ：LCD屏幕显示可选大小的汉字和字符混合
*函数参数  ：u16 x,u16 y,u16 color，u8 *str，u8 size ,u8 mode,u16 b_color 
*函数返回值：无
*函数描述  ：
			size 字号大小   16  32
			mode 传入1  就带背景颜色
			mode 传入0  就不带背景颜色
********************************************************************************/
void LCD_dis(u16 x,u16 y,u8 *str,u16 color,u8 mode,u16 b_color,u8 size){
	//遍历整个字符串
	while(*str != '\0'){
		//判断是字符
		//地址中存的是字符或者汉字的编码值
		if(*str>=32 && *str<=127){
			//判断位置是否可以显示,字符实际占用宽度为size/2
			//240-16/2=232。当x大于232时的时候，就需要换行，不够一个字符宽度
			if(x > (LCD_W-size/2)){
				x=0;
				y+=size;//换一行，字符实际占用高度为size
			}
			//取值传入字符
			LCD_dis_ch(x,y,*str,color,mode,b_color,size);
			str++;//下一个位置偏移一个字节，字符的话偏移一个字节，ASCII都在u8中
			x+=size/2;//一个字符的宽度为size/2
		}
		else{
			//不是字符就是汉字
			//判断位置是否可以显示,字符实际占用宽度为size/2
			//240-32=208。当x大于208时的时候，就需要换行，不够一个汉字宽度
			if(x > (LCD_W-size)){
				x=0;
				y+=size;//换一行，字符实际占用高度为size
			}
			LCD_dis_hz(x,y,str,color,mode,b_color,size);
			str+=2;//下一个位置,汉字的话偏移两个字节
			x+=size;//一个汉字的宽度为size
		}
	}
}



/**********************************************
*函数名    ：LCD_dis_pic
*函数功能  ：LCD屏幕显示任意大小图片
*函数参数  ：u16 x,u16 y,u8 *pic
*函数返回值：无
*函数描述  ：跟清屏函数一样，只不过每一个点的颜色值是图片的颜色值，不是都是背景色
*图片取模是像素点的颜色值，字符和汉字取模是用来判断是否需要打点的
************************************************/
void LCD_dis_pic(u16 x,u16 y,const u8 *pic){
	//图片模数据头部信息，说明在取模软件中
	HEADCOLOR *head;
	//每一个像素点需要两个字节数据来表示颜色值，所以定义u16,偏移方便，++直接偏移两个字节
	u16 *p;

	//获取头部信息,强制类型转换，拿到对应头部信息
	head = (HEADCOLOR *)pic;
	//拿到图片像素点颜色数据，去掉头部信息余下的就是像素点颜色数据
	p = (u16*)(pic+sizeof(HEADCOLOR));


	/*确定区域*/
	//确定x方向
	st7789vm_write_command(0x2A);     //横坐标命令
	//起始横坐标
	st7789vm_write_data(x);
	//结束横坐标
	//假设宽度是240的话，那么x~x+240-1刚好是240个像素点
	st7789vm_write_data(x+head->w-1);
	
	//确定Y方向
	st7789vm_write_command(0x2B);     //纵坐标命令
	//起始纵坐标
	st7789vm_write_data(y);
	//结束纵坐标
	st7789vm_write_data(y+head->h-1);
	
	
	/*确定颜色*/
	st7789vm_write_command(0x2C);     //颜色命令
	//总像素点个数，等于需要填充的颜色次数
	//假设240*240个像素点
	for(u32 i=0;i<head->w*head->h;i++){
		//u16,把一个像素点的数据填充
		st7789vm_write_data(*p);
		//偏移到下一个像素点起始地址
		p++;
	}

}





