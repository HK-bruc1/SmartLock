#include "main.h"


//每一个使用IIC通信的器件都有不同的要求

/***********************************************
*函数名    :bs8116_iic_IO_init
*函数功能  :IIC所用IO口初始化配置
*函数参数  :无
*函数返回值:无
*函数描述  : PB6----SCL   通用开漏输出
            PB7----SDA   通用开漏输出
************************************************/
void bs8116_iic_IO_init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//端口时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;          //通用输出
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;         //开漏
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 ;  // 6    7
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;       //无上下拉
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;       //速度
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	
	//空闲状态
	GPIO_SetBits(GPIOB, GPIO_Pin_6);//SCL
	GPIO_SetBits(GPIOB, GPIO_Pin_7);//SDA
	
	
}




/***********************************************
*函数名    :iic_star
*函数功能  :IIC起始信号函数
*函数参数  :无
*函数返回值:无
*函数描述  :时钟线为高电平，数据线出现下降沿
************************************************/
void bs8116_iic_star(void)
{
	//时钟线拉低，才可以动数据线
	BS8116_IIC_SCL_L; 
	tim5Delay_Us(5);
	//数据线拉高，目的是为出现下降沿做准备
	BS8116_IIC_SDA_OUT_H;   
	//时钟线拉高  产生起始信号的条件
	BS8116_IIC_SCL_H;
	tim5Delay_Us(5);
	//数据线拉低  可以产生起始信号
	BS8116_IIC_SDA_OUT_L;
	tim5Delay_Us(5);
	
	//安全
	BS8116_IIC_SCL_L;
	
}




/***********************************************
*函数名    :iic_stop
*函数功能  :IIC停止信号函数
*函数参数  :无
*函数返回值:无
*函数描述  :时钟线为高电平，数据线出现上升沿
************************************************/
void bs8116_iic_stop(void)
{
	
	//准备工作
	BS8116_IIC_SCL_L;
	BS8116_IIC_SDA_OUT_L;
	
	//产生停止信号
	BS8116_IIC_SCL_H;
	tim5Delay_Us(5);
	BS8116_IIC_SDA_OUT_H;
	tim5Delay_Us(5);

}

/***********************************************
*函数名    :iic_send_ack
*函数功能  :发送应答/不应答信号
*函数参数  :u8 ack
*函数返回值:无
*函数描述  :应答： 时钟线为高电平，数据线已经保持好低电平了
						不应答 时钟线为高电平，数据线已经保持好高电平了				
						参数：0   发送应答信号
						参数：1   发送不应答信号
************************************************/
void bs8116_iic_send_ack(u8 ack)
{
		BS8116_IIC_SCL_L;      //时钟线拉低，才可以动数据线
		tim5Delay_Us(3);
		if(ack == 0)       //发送应答
		{
			BS8116_IIC_SDA_OUT_L;  //让数据线先保持低电平
		}
		else if(ack == 1)
		{
			BS8116_IIC_SDA_OUT_H;  //让数据线先保持高电平
		}
		tim5Delay_Us(2);
		BS8116_IIC_SCL_H;      //产生了应答信号	
		tim5Delay_Us(5);
	
	
	//安全
	BS8116_IIC_SCL_L;
	
}



/***********************************************
*函数名    :iic_get_ack
*函数功能  :检测应答/不应答信号
*函数参数  :无
*函数返回值:u8
*函数描述  :
						返回0 表示应答信号
						返回1 表示不应答信号
************************************************/
u8 bs8116_iic_get_ack(void)
{
	
	u8 ack = 0;
	
	//数据线要为输入
	BS8116_IIC_SCL_L;
	//数据线切换输入状态
	BS8116_IIC_SDA_OUT_H;    
	
	//检测应答/不应答信号
	BS8116_IIC_SCL_L;   //主机帮助从机拉低时钟线，从机才可以动数据线，来表现应答/不应答信号
	tim5Delay_Us(5);  
	BS8116_IIC_SCL_H;   //主机才可以读数据线
	tim5Delay_Us(5);
	if(BS8116_IIC_SDA_INT == 0)
	{
		//低电平——应答信号
		ack = 0;
	}
	else
	{
		//高电平----不应答信号
		ack = 1;
	}
	
	//安全，BS8116独特要求，此时从机来拉低，不需要主机来拉低
	//BS8116_IIC_SCL_L;
	return ack;
	
}


/***********************************************
*函数名    :iic_send_byte
*函数功能  :IIC发送一个字节函数
*函数参数  :u8 data
*函数返回值:无
*函数描述  :
************************************************/
void bs8116_iic_send_byte(u8 data)
{
	u8 i;
	for(i=0;i<8;i++)
	{
		//时钟线拉低才可以改变数据线
		BS8116_IIC_SCL_L;
		tim5Delay_Us(3);  
		//根据参数的对应位是0还是1决定数据线的高低
		if(data & 0x80)
		{
			BS8116_IIC_SDA_OUT_H;
		}
		else
		{
			BS8116_IIC_SDA_OUT_L;
		}
		tim5Delay_Us(2);  
		//时钟下拉高，帮助从机，这样从机才可以读这一位数据
		BS8116_IIC_SCL_H;
		tim5Delay_Us(5);  
		//下一位数据，次高位变成最高位
		data = data << 1;
	}
	//安全
	BS8116_IIC_SCL_L;
	
}


/*******************************************
*函数名    :iic_read_byte
*函数功能  :主机通过IIC接收一个字节函数
*函数参数  :无
*函数返回值:u8
*函数描述  :
*********************************************/
u8 bs8116_iic_read_byte(void)
{
	
	u8 data;    
	u8 i;
	
	//将IO口切换为输入
	BS8116_IIC_SCL_L;
	BS8116_IIC_SDA_OUT_H;
	
	//读数据
	for(i=0;i<8;i++)
	{
		BS8116_IIC_SCL_L;             //主机帮助从机拉低时钟线
		tim5Delay_Us(5);   
		BS8116_IIC_SCL_H;             //主机可以读数据
		tim5Delay_Us(5);
		data = data << 1;
		if(BS8116_IIC_SDA_INT) 
			data= data | 0x01;	
		
	}
	
	//安全
	BS8116_IIC_SCL_L;
	
	return data;
	
}






/***********************************************
*函数名    :BS8116_init
*函数功能  :BS8116初始化配置
*函数参数  :无
*函数返回值:无
*函数描述  :IRQ-----PA1
************************************************/
void BS8116_init(void)
{
	
	u8 register_buff[21] = {0};
	
	//BS8116模块的初始配置
	u8 write_buff[21] = {	0x00,0x00,0x83,0xf3,0x98,
							0x3f,0x3f,0x3f,0x3f,0x3f,
							0x3f,0x3f,0x3f,0x3f,0x3f,
							0x3f,0x3f,0x3f,0x3f,0x3f,0x7f};	
	
	/*IIC所用IO口初始化*/
	bs8116_iic_IO_init();

	/*IRQ所用IO口初始化,余下默认，检测是否按下按键的*/
	RCC->AHB1ENR |=0x01<<0;//端口时钟使能
	GPIOA->MODER &= ~(3<<2);
											
	


	/*寄存器初始配置,假设先前状态并不是想要的*/									
	BS8116_Writeregister(write_buff);
	
												
	//读取寄存器的值(查看配置的寄存器的值),睡眠状态下不是真的配置参数，需要先唤醒再复位
	// BS8116_Readregister(register_buff);
	// for(u8 i=0;i<21;i++){
	// 	printf("0x%x\r\n",register_buff[i]);
	// }

	
	
}



/***********************************************
*函数名    :BS8116_Writeregister
*函数功能  :BS8116写寄存器的值
*函数参数  :u8 *cmd_buff
*函数返回值:u8      //ack
*函数描述  :
************************************************/
u8 BS8116_Writeregister(u8 *cmd_buff)
{
	//用来发送校验和
	u32 check_sum = 0;
	
	//开始信号并发送器件地址
	bs8116_iic_star();
	bs8116_iic_send_byte(BS8116ADDR_W);  
	//检测从机应答
	if(bs8116_iic_get_ack())
	{
		bs8116_iic_stop( );
		return 1;
	}
	//等待从机不忙，从机会处理命令，从机处理完后，会拉高SCL线
	while(!BS8116_IIC_SCL_INT);
	
	//配置寄存器的设置
	bs8116_iic_send_byte(0xB0);    //寄存器的起始地址 
	if(bs8116_iic_get_ack())
	{
		bs8116_iic_stop( );
		return 2;
	}
	//等待从机不忙
	while(!BS8116_IIC_SCL_INT);
	

	/*发送寄存器配置数据*/
	//发送21个寄存器数据和一个累加和数据
	for(u8 i=1;i<=22;i++)
	{
		//第22次发送累加和
		if(i==22)
		{
			bs8116_iic_send_byte(check_sum & 0xff);     //发送校验和 低八位
		}
		//21之前要发送寄存器数据并且累加
		else
		{
			bs8116_iic_send_byte(*cmd_buff);
			check_sum = check_sum + *cmd_buff;         //累加
			cmd_buff++;
		}
	
		//主机检测应答
		if(bs8116_iic_get_ack())
		{
			bs8116_iic_stop( );
			return 3;
		}
		
		//等待从机不忙
		while(!BS8116_IIC_SCL_INT);
	}
	
	bs8116_iic_stop( );
	
	return 0;
}




/***********************************************
*函数名    :BS8116_Readregister
*函数功能  :BS8116读寄存器的值
*函数参数  :u8 *buff 
*函数返回值:u8 
*函数描述  :读到的21个寄存器的值，用于检测是否配置成功
************************************************/
u8 BS8116_Readregister(u8 *buff)
{

	bs8116_iic_star();
	bs8116_iic_send_byte(BS8116ADDR_W);  
	if(bs8116_iic_get_ack())
	{
		bs8116_iic_stop( );
		return 1;
	}
	
	//从机把时钟拉低表示正在处理数据
	//主机需要等待从机把时钟线拉高
	while(!BS8116_IIC_SCL_INT);
	
	
	bs8116_iic_send_byte(0xB0);     //从起始寄存器地址开始
	if(bs8116_iic_get_ack())
	{
		bs8116_iic_stop( );
		return 2;
	}
	//等待从机不忙
	while(!BS8116_IIC_SCL_INT);
	
	bs8116_iic_star();
	bs8116_iic_send_byte(BS8116ADDR_R);  
	if(bs8116_iic_get_ack())
	{
		bs8116_iic_stop( );
		return 3;
	}
	//等待从机不忙
	while(!BS8116_IIC_SCL_INT);
	
	//读21个配置寄存器数据
	for(u8 i=0;i<21;i++)      
	{
		*buff = bs8116_iic_read_byte();
		if(i==20) bs8116_iic_send_ack(1);
		else     bs8116_iic_send_ack(0);
		buff++;
	}
	bs8116_iic_stop( );
	
	return 0;
}


/***********************************************
*函数名    :BS8116_ReadKey
*函数功能  :BS8116读键值
*函数参数  :无
*函数返回值:u8
*函数描述  :
************************************************/
u8 BS8116_ReadKey(void)
{
	u16 key;
	
	bs8116_iic_star();
	bs8116_iic_send_byte(BS8116ADDR_W);  
	if(bs8116_iic_get_ack())
	{
		bs8116_iic_stop( );
		return 1;
	}
	//等待从机不忙
	while(!BS8116_IIC_SCL_INT);

	bs8116_iic_send_byte(0x08);    //发送键值寄存器的地址
	if(bs8116_iic_get_ack())
	{
		bs8116_iic_stop( );
		return 2;
	}
	//等待从机不忙
	while(!BS8116_IIC_SCL_INT);
	
	bs8116_iic_star();
	bs8116_iic_send_byte(BS8116ADDR_R);
	if(bs8116_iic_get_ack())
	{
		bs8116_iic_stop( );
		return 3;
	}  
	//等待从机不忙
	while(!BS8116_IIC_SCL_INT);
	
	key=bs8116_iic_read_byte();   //08
	bs8116_iic_send_ack(0);        //继续读
	key = (bs8116_iic_read_byte()<<8) | key;//先拿的是1~8号按键，再拿的是9~16号按键
	bs8116_iic_send_ack(1);        //不需要数据了
	bs8116_iic_stop( );
	//key值就是键值寄存器的总体数据
	printf("key:0x%x\r\n",key);	

	//重新映射键值，因为物理键盘并不对应BS8116的寄存器键值，布线困难
	
	
	
	return 0xff;
}















