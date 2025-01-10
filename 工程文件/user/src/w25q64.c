#include "main.h"                  // Device header

/***********************************************
*函数名    ：w25q64_spi_init
*函数功能  ：w25q64所用SPI初始化配置SPI1
*函数参数  ：无
*函数返回值：无
*函数描述  ：CLK         时钟线：PA5  //复用推挽
						 DO（SO）数据输出线：PA6  //复用推挽
						 DI（SI） 数据输入线：PA7  //复用
***********************************************/
void w25q64_spi_init(void)
{
	/*IO口控制器配置*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);	
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Fast_Speed;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_SPI1);
	

	/*SPI控制器配置*/	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
	
	SPI_InitTypeDef SPI_InitStruct = {0};
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
	
	SPI_Init(SPI1, &SPI_InitStruct);
	
	//使能SPI
	SPI_Cmd(SPI1, ENABLE);
}



/************************************************
函数名  : w25q64_Spi_bayte
函数功能：SPI发送和接收数据
函数形参：u8 data
函数返回值：u8 
说明：高位先发
************************************************/
u8 w25q64_Spi_bayte(u8 data)
{
	u8 buf = 0;
	//等到发送寄存器为空就可以写入数据
	while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE));
	
	SPI_I2S_SendData(SPI1,data);
	//等到接收数据寄存器不为空就可以读取数据
	while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE));
	
	buf = SPI_I2S_ReceiveData(SPI1);
	return buf;
}






/***********************************************
*函数名    ：w25q64_init
*函数功能  ：w25q64初始化配置
*函数参数  ：无
*函数返回值：无
*函数描述  ：CS:PA4
***********************************************/
void w25q64_init(void)
{
	//SPI初始化配置
	w25q64_spi_init();
	//片选线配置
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;          //通用输出
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;         //推挽
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4 ; 
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;       //无上下拉
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;       //速度
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_SetBits(GPIOA, GPIO_Pin_4);                   //片选拉高
	
	
	//解除块保护
	write_status();
	
}


/***********************************************
*函数名    ：read_status
*函数功能  ：读w25q64状态寄存器的值
*函数参数  ：无
*函数返回值：u8
*函数描述  ：命令0x05
***********************************************/
u8 read_status(void)
{
	u8 sta;
	W25Q64_CS_L;
	w25q64_Spi_bayte(0x05);
	sta = w25q64_Spi_bayte(0xff);
	W25Q64_CS_H;
	
	return sta;
}


/***********************************************
*函数名    ：wirite_enable
*函数功能  ：写使能
*函数参数  ：无
*函数返回值：无
*函数描述  ：命令0x06
***********************************************/
void wirite_enable(void)
{
	W25Q64_CS_L;
	w25q64_Spi_bayte(0x06);
	W25Q64_CS_H;
}




/***********************************************
*函数名    ：write_status
*函数功能  ：写w25q64状态寄存器，解除所有保护
*函数参数  ：无
*函数返回值：u8
*函数描述  ：命令0x01
***********************************************/
void write_status(void)
{
	wirite_enable();
	W25Q64_CS_L;
	w25q64_Spi_bayte(0x01);
	w25q64_Spi_bayte(0x00);      
	W25Q64_CS_H;
	//读状态寄存器的busy位等待写完
	while((read_status()) & 0x01);
}


/***********************************************
*函数名    ：w25q64_page_write
*函数功能  ：往w25q64中写数据 页写
*函数参数  ：u32 inner_addr,u8 num_byte,u8 *str
*函数返回值：无
*函数描述  ：命令0x02
***********************************************/
u8 w25q64_page_write(u32 inner_addr,u8 num_byte,u8 *str)
{
	
	/*判断要写的内容是否会跨页*/
	if(inner_addr / 256 != (inner_addr + num_byte - 1) / 256)
	{
		return 0;
	}
	
	wirite_enable();
	W25Q64_CS_L;
	w25q64_Spi_bayte(0x02); 
	w25q64_Spi_bayte(inner_addr>>16); 
	w25q64_Spi_bayte(inner_addr>>8);  
	w25q64_Spi_bayte(inner_addr&0xff); 
	while(1)
	{
		w25q64_Spi_bayte(*str); 
		num_byte--;
		if(num_byte == 0) break;
		str++;	
	}
	W25Q64_CS_H;
	//读状态寄存器busy，等待数据写入完成
	while((read_status()) & 0x01);
	
	return 1;
}


/***********************************************
*函数名    ：w25q64_read
*函数功能  ：从w25q64中读数据
*函数参数  ：u32 inner_addr,u32 num_byte,u8 *str
*函数返回值：无
*函数描述  ：命令0x03
***********************************************/
void w25q64_read(u32 inner_addr,u32 num_byte,u8 *str)
{
	W25Q64_CS_L;
	w25q64_Spi_bayte(0x03);
	w25q64_Spi_bayte(inner_addr>>16); 
	w25q64_Spi_bayte(inner_addr>>8);  
	w25q64_Spi_bayte(inner_addr&0xff); 
	while(1)
	{
		*str = w25q64_Spi_bayte(0xff);
		num_byte--;
		if(num_byte == 0)
		{
			break;
		}
		str++;
	}
	W25Q64_CS_H;	
}




/***********************************************
*函数名    ：w25q64_sector_erase
*函数功能  ：擦除传入地址的所在扇区
*函数参数  ：u32 inner_addr
*函数返回值：无
*函数描述  ：命令0x20
***********************************************/
void w25q64_sector_erase(u32 inner_addr)
{
	wirite_enable();
	W25Q64_CS_L;
	w25q64_Spi_bayte(0x20); 
	w25q64_Spi_bayte(inner_addr>>16); 
	w25q64_Spi_bayte(inner_addr>>8);  
	w25q64_Spi_bayte(inner_addr&0xff); 
	W25Q64_CS_H;
	//读状态寄存器busy，等待数据写入完成
	while((read_status()) & 0x01);
}

/***********************************************
*函数名    ：w25q64_block_erase
*函数功能  ：擦除传入地址的所在块区
*函数参数  ：u32 inner_addr
*函数返回值：无
*函数描述  ：命令0xD8
***********************************************/
void w25q64_block_erase(u32 inner_addr)
{
	wirite_enable();
	W25Q64_CS_L;
	w25q64_Spi_bayte(0xD8); 
	w25q64_Spi_bayte(inner_addr>>16); 
	w25q64_Spi_bayte(inner_addr>>8);  
	w25q64_Spi_bayte(inner_addr&0xff); 
	W25Q64_CS_H;
	//读状态寄存器busy，等待数据写入完成
	while((read_status()) & 0x01);
}

/***********************************************
*函数名    ：w25q64_chip_erase
*函数功能  ：芯片擦除
*函数参数  ：无
*函数返回值：无
*函数描述  ：命令0xC7
***********************************************/
void w25q64_chip_erase(void)
{
	wirite_enable();
	W25Q64_CS_L;
	w25q64_Spi_bayte(0xC7); 
	W25Q64_CS_H;
	//读状态寄存器busy，等待数据写入完成
	while((read_status()) & 0x01);
}






/***********************************************
*函数名    ：w25q64_blocks
*函数功能  ：多块擦除
*函数参数  ：u32 inner_addr,u8 blocks
*函数返回值：无
*函数描述  ：
***********************************************/
void w25q64_blocks(u32 inner_addr,u8 blocks)
{
	while(blocks)
	{
		w25q64_block_erase(inner_addr);
		inner_addr += 65536;
		blocks--;
	}
}


//W25Q64测试函数
void W25Q64_Test(void){
    u8 writeData[300] = {0}; // 增加数组大小，确保跨页
    u8 readData[300] = {0};  // 读取数据缓冲区
    
    // 初始化W25Q64控制器
    w25q64_init();
    printf("W25Q64 Test Start...\r\n");
    
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

    // 擦除扇区数据
    w25q64_sector_erase(0x0000FA);

    // 写入超过256字节的数据，测试跨页
    w25q64_page_write(0x000010, 200, writeData);
    printf("Write 300 Bytes Data\r\n");
    
    // 读取数据
    w25q64_read(0x000010, 200, readData);
    printf("Read 300 Bytes Data\r\n");
    
    // 打印readData数组的前16个元素
    printf("ReadData: ");
    for (int i = 0; i < 16; i++) {
        printf("%02X ", readData[i]);
    }
    printf("\r\n");
    
    // 比较写入和读取的数据
    int errorCount = 0;
    for (int i = 0; i < 300; i++) {
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






