#include "main.h"                  // Device header

/***********************************************
*函数名    :iic_IO_init
*函数功能  :IIC所用IO口初始化配置
*函数参数  :无
*函数返回值:无
*函数描述  :PA8----SCL   通用推挽输出
            PC9----SDA   通用开漏输出
************************************************/
void iic_IO_init(void)
{
	RCC->AHB1ENR |= (1<<0);
	RCC->AHB1ENR |= (1<<2);
	GPIOA->MODER &= ~(3<<16) ;
	GPIOA->MODER |= (1<<16) ;
	GPIOC->MODER &= ~(3<<18) ;
	GPIOC->MODER |= (1<<18) ;
	//PA8推挽
	GPIOA->OTYPER &= ~(1<<8);
	//PC9开漏
	GPIOC->OTYPER |= (1<<9);
	
	//空闲状态
	IIC_SCL_H;
	IIC_SDA_OUT_H;
}



/***********************************************
*函数名    :iic_star
*函数功能  :IIC起始信号函数
*函数参数  :无
*函数返回值:无
*函数描述  :时钟线为高电平，数据线出现下降沿
************************************************/

void iic_star(void)
{
	//时钟线拉低，才可以动数据线
	IIC_SCL_L;  
	//数据线拉高，目的是为出现下降沿做准备
	IIC_SDA_OUT_H;   
	//时钟线拉高  产生起始信号的条件
	IIC_SCL_H;
	tim5Delay_Us(5);
	//数据线拉低  可以产生起始信号
	IIC_SDA_OUT_L;
	tim5Delay_Us(5);
	
	//安全
	IIC_SCL_L;
}



/***********************************************
*函数名    :iic_stop
*函数功能  :IIC停止信号函数
*函数参数  :无
*函数返回值:无
*函数描述  :时钟线为高电平，数据线出现上升沿
************************************************/
void iic_stop(void)
{
	//准备工作
	IIC_SCL_L;
	IIC_SDA_OUT_L;
	
	//产生停止信号
	IIC_SCL_H;
	tim5Delay_Us(5);
	IIC_SDA_OUT_H;
	tim5Delay_Us(5);
}



/***************************************
*函数名			：iic_send_ack
*函数功能		：IIC发送应答/不应答信号
*函数参数		：u8 ack
*函数返回值	：无
*函数描述		：ack 传入 0   表示发送应答
              ack 传入 1   表示发送不应答
****************************************/
void iic_send_ack(u8 ack)
{
	IIC_SCL_L;      //可以动数据线
	tim5Delay_Us(3);
	ack ? (IIC_SDA_OUT_H) : (IIC_SDA_OUT_L);
	tim5Delay_Us(2);
	
	IIC_SCL_H;      //产生应答/不应答信号
	tim5Delay_Us(5);
	IIC_SCL_L;     //安全作用
}


/***************************************
*函数名			：iic_rec_ack
*函数功能		：IIC检测应答/不应答信号
*函数参数		：无
*函数返回值	：u8
*函数描述		：返回 0  表示应答
              返回 1  表示不应答
****************************************/
u8 iic_get_ack(void)
{
	u8 ack;
	
	/*将数据线切换为输入状态*/
	IIC_SCL_L;      //可以动数据线
	IIC_SDA_OUT_H;
	
	/*读取应答信号*/
	IIC_SCL_L;      //主机帮助从机拉低时钟线，从机才能改变数据
	tim5Delay_Us(5);
	IIC_SCL_H;
	tim5Delay_Us(5);
	if(IIC_SDA_INT)
	{
		ack = 1;
	}
	else 
	{
		ack = 0;
	}
	
	
	IIC_SCL_L;    //安全作用
	
	return ack;
	
}



/***************************************
*函数名			：iic_send_byte
*函数功能		：IIC发送一个字节数据函数
*函数参数		：u8 data
*函数返回值	：无
*函数描述		：
****************************************/
void iic_send_byte(u8 data)
{
	u8 i;
	for(i=0;i<8;i++)
	{
		IIC_SCL_L;      //可以动数据线
		tim5Delay_Us(3);
		if(data & 0x80)
		{
			IIC_SDA_OUT_H;
		}
		else 
		{
			IIC_SDA_OUT_L;
		}
		tim5Delay_Us(2);
		
		IIC_SCL_H;     //帮助从机拉高时钟线，从机可以读此位数据
		tim5Delay_Us(5);
		
		//下一位数据
		data = data << 1;
	}
	
	
	IIC_SCL_L;    //安全作用
	
}



/***************************************
*函数名			：iic_rec_byte
*函数功能		：IIC接收一个字节数据函数
*函数参数		：无
*函数返回值	：u8
*函数描述		：
****************************************/
u8 iic_read_byte(void)
{
	u8 i;
	u8 data;
	
	/*将数据线改为输入状态*/
	IIC_SCL_L;      //可以动数据线
	IIC_SDA_OUT_H;
	
	/*接收8位数据*/
	for(i=0;i<8;i++)
	{
		IIC_SCL_L;     //帮助从机把时钟线拉低，从机就可以改变数据线
		tim5Delay_Us(5);
		IIC_SCL_H;     //主机可以读数据
		tim5Delay_Us(5);
		data = data << 1;
		if(IIC_SDA_INT)
		{
			data |= 0x01;
		}
	}

	IIC_SCL_L;    //安全作用
	
	
	return data;
	
}



/********************************************************
*函数名    :at24c0x_init
*函数功能  :AT24C0X的初始化
*函数参数  :
*函数返回值:
**********************************************************/
void at24c0x_init(void)
{
	iic_IO_init();
}


/********************************************************
*函数名    :at24c0x_write_byte
*函数功能  :往at24c0x的某个地址空间写一个字节数据
*函数参数  :u16 inner_addr     //器件内部存储空间地址  0~511   
						u8 data           //要写入的数据
*函数返回值:每次发送都会检测响应，通过返回值判断具体哪次发送出问题			
*函数描述  :宏定义错误标志：
							AT24C0X_NO_ERR 0;   //应答  无错误
							AT24C0X_ERR1   1;   //无应答
							AT24C0X_ERR2   2;   //无应答
							AT24C0X_ERR3   3;   //无应答
**********************************************************/
u8 at24c0x_write_byte(u16 inner_addr,u8 data)
{
	u8 ack;
	
	iic_star();       
	                      
	iic_send_byte(AT24C0x_ADDR_W+inner_addr/256*2); 
	
	ack = iic_get_ack();
	if(ack == 1)
	{
		iic_stop();
		return AT24C0X_ERR1; 
	}
	
	/*器件绝对地址*/
	iic_send_byte(inner_addr % 256);   //真实内部地址
	ack = iic_get_ack();
	if(ack == 1)
	{
		iic_stop();
		return AT24C0X_ERR2; 
	}
	
	
	//发送数据
	iic_send_byte(data);
	ack = iic_get_ack();
	if(ack == 1)
	{
		iic_stop();
		return AT24C0X_ERR3; 
	}
	iic_stop();
	tim5Delay_Ms(5);
	
	return AT24C0X_NO_ERR;
}




/********************************************************
*函数名    :at24c0x_read_byte
*函数功能  :从at24c0x的某个地址空间读一个字节数据
*函数参数  :u16 inner_addr     //器件内部存储空间地址  0~511   
						u8 *data           //读入数据存放地址
*函数返回值:每次发送都会检测响应，通过返回值判断具体哪次发送出问题			
*函数描述  :宏定义错误标志：
							AT24C0X_NO_ERR 0;   //应答  无错误
							AT24C0X_ERR1   1;   //无应答
							AT24C0X_ERR2   2;   //无应答
							AT24C0X_ERR3   3;   //无应答
**********************************************************/
u8 at24c0x_read_byte(u16 inner_addr,u8 *data)
{
	u8 ack;
	
	iic_star();
	iic_send_byte(AT24C0x_ADDR_W+inner_addr/256*2); 
	ack = iic_get_ack();
	if(ack == 1)
	{
		iic_stop();
		return AT24C0X_ERR1; 
	}
	
	/*发送绝对地址*/
	iic_send_byte(inner_addr&0xff);
	//inner_addr = inner_addr & 0xff;    //第二块的实际绝对地址
	//inner_addr = inner_addr % 256;    //第二块的实际绝对地址
	ack = iic_get_ack();
	if(ack == 1)
	{
		iic_stop();
		return AT24C0X_ERR2; 
	}
	 
	
	iic_star();
	
	iic_send_byte(AT24C0x_ADDR_R+inner_addr/256*2);
	ack = iic_get_ack();
	if(ack == 1)
	{
		iic_stop();
		return AT24C0X_ERR1; 
	}	
	
	*data = iic_read_byte();
	iic_send_ack(1);
	iic_stop();
	
	return AT24C0X_NO_ERR;
}





/********************************************************
*函数名    :AT24C0x_read_bytes
*函数功能  :从AT24C0x的某个地址空间开始连续读多个字节数据
*函数参数  :u16 inner_addr     //器件内部存储空间地址  0~511  
            u16 num            //要读多少个字节
						u8 *str           //读到的数据存放的地址
*函数返回值:每次发送都会检测响应，通过返回值判断具体哪次发送出问题			
*函数描述  :宏定义错误标志：
							AT24C0X_NO_ERR 0;   //应答  无错误
							AT24C0X_ERR1   1;   //无应答
							AT24C0X_ERR2   2;   //无应答
							AT24C0X_ERR3   3;   //无应答
**********************************************************/
u8 AT24C0x_read_bytes(u16 inner_addr,u16 num,u8 *str)
{
	u8 ack;
	
	iic_star();
	
	iic_send_byte(AT24C0x_ADDR_W+inner_addr/256*2);    
	ack = iic_get_ack();
	if(ack == 1)
	{
		iic_stop();
		return AT24C0X_ERR1; 
	}	
	
	/*发送绝对地址*/
	iic_send_byte(inner_addr&0xff);
	ack = iic_get_ack();
	if(ack == 1)
	{
		iic_stop();
		return AT24C0X_ERR2; 
	}
	
	iic_star();
	iic_send_byte(AT24C0x_ADDR_R+inner_addr/256*2);    
	ack = iic_get_ack();
	if(ack == 1)
	{
		iic_stop();
		return AT24C0X_ERR1; 
	}	
	
	//循环读数据
	while(1)
	{
		*str = iic_read_byte();       
		num--;                         
		if(num == 0)
		{
			iic_send_ack(1);
			break;
		}
		iic_send_ack(0);
		str++;
	}
	
	iic_stop();
	
	return AT24C0X_NO_ERR;
}


/********************************************************
*函数名    :AT24C0x_write_page
*函数功能  :往AT24C0x的某个地址空间开始页写
*函数参数  :u16 inner_addr     //器件内部存储空间地址  0~511  
            u8 num            //要读多少个字节
						u8 *str           //要写的字符串的首地址
*函数返回值:每次发送都会检测响应，通过返回值判断具体哪次发送出问题			
*函数描述  :宏定义错误标志：
							AT24C0X_NO_ERR 0;   //应答  无错误
							AT24C0X_ERR1   1;   //无应答
							AT24C0X_ERR2   2;   //无应答
							AT24C0X_ERR3   3;   //无应
**********************************************************/
u8 AT24C0x_write_page(u16 inner_addr,u8 num,u8 *str)
{
	u8 ack;
	
	
	/*页写*/
	iic_star();
	iic_send_byte(AT24C0x_ADDR_W+inner_addr/256*2); 
	ack = iic_get_ack();
	if(ack == 1)
	{
		iic_stop();
		return AT24C0X_ERR1; 
	}	
	
	/*发送绝对地址*/
	iic_send_byte(inner_addr&0xff);
	ack = iic_get_ack();
	if(ack == 1)
	{
		iic_stop();
		return AT24C0X_ERR2; 
	}
	
	while(1)
	{
		iic_send_byte(*str);
		ack = iic_get_ack();
		if(ack == 1)
		{
			iic_stop();
			return AT24C0X_ERR3; 
		}	
		num--;
		if(num == 0)
		{
			break;
		}
		str++;
	}
	iic_stop();
	
	//写周期
	tim5Delay_Ms(5);
	
	return AT24C0X_NO_ERR;
}


/********************************************************
*函数名    :AT24C0x_write_bytes
*函数功能  :往AT24C0X的某个地址空间开始连续写可以跨页
*函数参数  :u16 inner_addr     //器件内部存储空间地址  0~511  
            u16 num            //要读多少个字节
						u8 *str           //要写的字符串的首地址
						u8 AT_flag        //传1就是  AT24c02    传2就是AT24c04  
*函数返回值:每次发送都会检测响应，通过返回值判断具体哪次发送出问题			
*函数描述  :宏定义错误标志：
							AT24C0X_NO_ERR 0;   //应答  无错误
							AT24C0X_ERR1   1;   //无应答
							AT24C0X_ERR2   2;   //无应答
							AT24C0X_ERR3   3;   //无应
**********************************************************/
u8 AT24C0x_write_bytes(u16 inner_addr,u16 num,u8 *str,u8 AT_flag)
{
	u8 less_num;
	
	
	while(1)
	{
		//计算本页还剩多少个空间可以写
		less_num = 8*AT_flag - inner_addr % (8 * AT_flag);   
		
		//判断是否会跨页
		if(less_num >= num)       //不跨页
		{
			//调用页写函数将写完
			AT24C0x_write_page(inner_addr,num,str);  
			//退出循环结构
			break;
		}
		//跨页
		else
		{
			//先调用页写函数把本页写完
			AT24C0x_write_page(inner_addr,less_num,str);  // 
			//存储芯片切换到下页的起始位置位置
			inner_addr = inner_addr + less_num;   //
			
			//计算出剩余要写的个数
			num = num - less_num;                 //
			
			//计算出剩余要写的内容的地址
			str = str + less_num;	
		}
	}		
	return AT24C0X_NO_ERR;
}

//AT24C0XX测试函数
void AT24C0XX_Test(void){
    u8 writeData[300] = {0}; // 增加数组大小，确保跨页
    u8 readData[300] = {0};  // 读取数据缓冲区
    
    // 初始化at24c0x_init控制器
    at24c0x_init();
    printf("at24c0x Test Start...\r\n");
    
    // 填充测试数据
    for (int i = 0; i < 200; i++) {
        writeData[i] = i; // 填充0-199的数据
    }

    // 打印writeData数组的前16个元素
    printf("WriteData: ");
    for (int i = 0; i < 16; i++) {
        printf("%02X ", writeData[i]);
    }
    printf("\r\n");


    // 写入超过20字节的数据，测试跨页
    AT24C0x_write_bytes(0,200,writeData,AT24C04);
    printf("Write 200 Bytes Data\r\n");
    
    // 读取数据
    AT24C0x_read_bytes(0,200,readData);
    printf("Read 200 Bytes Data\r\n");
    
    // 打印readData数组的前16个元素
    printf("ReadData: ");
    for (int i = 0; i < 16; i++) {
        printf("%02X ", readData[i]);
    }
    printf("\r\n");
    
    // 比较写入和读取的数据
    int errorCount = 0;
    for (int i = 0; i < 200; i++) {
        if (writeData[i] != readData[i]) {
            errorCount++;
            // 可选：打印具体的错误位置和数据
            printf("Error at index %d: wrote %02X, read %02X\r\n", i, writeData[i], readData[i]);
        }
    }
    
    if (errorCount == 0) {
        printf("Data Verify Success!\r\n");
    } else {
        printf("Data Verify Failed! Error Count: %d\r\n", errorCount);
    }
}

























