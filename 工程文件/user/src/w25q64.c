#include "main.h"                  // Device header

/***********************************************
*������    ��w25q64_spi_init
*��������  ��w25q64����SPI��ʼ������SPI1
*��������  ����
*��������ֵ����
*��������  ��CLK         ʱ���ߣ�PA5  //��������
						 DO��SO����������ߣ�PA6  //��������
						 DI��SI�� ���������ߣ�PA7  //����
***********************************************/
void w25q64_spi_init(void)
{
	/*IO�ڿ���������*/
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
	

	/*SPI����������*/	
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
	
	//ʹ��SPI
	SPI_Cmd(SPI1, ENABLE);
}



/************************************************
������  : w25q64_Spi_bayte
�������ܣ�SPI���ͺͽ�������
�����βΣ�u8 data
��������ֵ��u8 
˵������λ�ȷ�
************************************************/
u8 w25q64_Spi_bayte(u8 data)
{
	u8 buf = 0;
	//�ȵ����ͼĴ���Ϊ�վͿ���д������
	while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE));
	
	SPI_I2S_SendData(SPI1,data);
	//�ȵ��������ݼĴ�����Ϊ�վͿ��Զ�ȡ����
	while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE));
	
	buf = SPI_I2S_ReceiveData(SPI1);
	return buf;
}






/***********************************************
*������    ��w25q64_init
*��������  ��w25q64��ʼ������
*��������  ����
*��������ֵ����
*��������  ��CS:PA4
***********************************************/
void w25q64_init(void)
{
	//SPI��ʼ������
	w25q64_spi_init();
	//Ƭѡ������
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;          //ͨ�����
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;         //����
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4 ; 
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;       //��������
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;       //�ٶ�
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_SetBits(GPIOA, GPIO_Pin_4);                   //Ƭѡ����
	
	
	//����鱣��
	write_status();
	
}


/***********************************************
*������    ��read_status
*��������  ����w25q64״̬�Ĵ�����ֵ
*��������  ����
*��������ֵ��u8
*��������  ������0x05
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
*������    ��wirite_enable
*��������  ��дʹ��
*��������  ����
*��������ֵ����
*��������  ������0x06
***********************************************/
void wirite_enable(void)
{
	W25Q64_CS_L;
	w25q64_Spi_bayte(0x06);
	W25Q64_CS_H;
}




/***********************************************
*������    ��write_status
*��������  ��дw25q64״̬�Ĵ�����������б���
*��������  ����
*��������ֵ��u8
*��������  ������0x01
***********************************************/
void write_status(void)
{
	wirite_enable();
	W25Q64_CS_L;
	w25q64_Spi_bayte(0x01);
	w25q64_Spi_bayte(0x00);      
	W25Q64_CS_H;
	//��״̬�Ĵ�����busyλ�ȴ�д��
	while((read_status()) & 0x01);
}


/***********************************************
*������    ��w25q64_page_write
*��������  ����w25q64��д���� ҳд
*��������  ��u32 inner_addr,u8 num_byte,u8 *str
*��������ֵ����
*��������  ������0x02
***********************************************/
u8 w25q64_page_write(u32 inner_addr,u8 num_byte,u8 *str)
{
	
	/*�ж�Ҫд�������Ƿ���ҳ*/
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
	//��״̬�Ĵ���busy���ȴ�����д�����
	while((read_status()) & 0x01);
	
	return 1;
}


/***********************************************
*������    ��w25q64_read
*��������  ����w25q64�ж�����
*��������  ��u32 inner_addr,u32 num_byte,u8 *str
*��������ֵ����
*��������  ������0x03
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
*������    ��w25q64_sector_erase
*��������  �����������ַ����������
*��������  ��u32 inner_addr
*��������ֵ����
*��������  ������0x20
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
	//��״̬�Ĵ���busy���ȴ�����д�����
	while((read_status()) & 0x01);
}

/***********************************************
*������    ��w25q64_block_erase
*��������  �����������ַ�����ڿ���
*��������  ��u32 inner_addr
*��������ֵ����
*��������  ������0xD8
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
	//��״̬�Ĵ���busy���ȴ�����д�����
	while((read_status()) & 0x01);
}

/***********************************************
*������    ��w25q64_chip_erase
*��������  ��оƬ����
*��������  ����
*��������ֵ����
*��������  ������0xC7
***********************************************/
void w25q64_chip_erase(void)
{
	wirite_enable();
	W25Q64_CS_L;
	w25q64_Spi_bayte(0xC7); 
	W25Q64_CS_H;
	//��״̬�Ĵ���busy���ȴ�����д�����
	while((read_status()) & 0x01);
}






/***********************************************
*������    ��w25q64_blocks
*��������  ��������
*��������  ��u32 inner_addr,u8 blocks
*��������ֵ����
*��������  ��
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


//W25Q64���Ժ���
void W25Q64_Test(void){
    u8 writeData[300] = {0}; // ���������С��ȷ����ҳ
    u8 readData[300] = {0};  // ��ȡ���ݻ�����
    
    // ��ʼ��W25Q64������
    w25q64_init();
    printf("W25Q64 Test Start...\r\n");
    
    // ����������
    for (int i = 0; i < 200; i++) {
        writeData[i] = i; // ���0-199������
    }

    // ��ӡwriteData�����ǰ16��Ԫ��
    printf("WriteData: ");
    for (int i = 0; i < 16; i++) {
        printf("%02X ", writeData[i]);
    }
    printf("\r\n");

    // ������������
    w25q64_sector_erase(0x0000FA);

    // д�볬��256�ֽڵ����ݣ����Կ�ҳ
    w25q64_page_write(0x000010, 200, writeData);
    printf("Write 300 Bytes Data\r\n");
    
    // ��ȡ����
    w25q64_read(0x000010, 200, readData);
    printf("Read 300 Bytes Data\r\n");
    
    // ��ӡreadData�����ǰ16��Ԫ��
    printf("ReadData: ");
    for (int i = 0; i < 16; i++) {
        printf("%02X ", readData[i]);
    }
    printf("\r\n");
    
    // �Ƚ�д��Ͷ�ȡ������
    int errorCount = 0;
    for (int i = 0; i < 300; i++) {
        if (writeData[i] != readData[i]) {
            errorCount++;
            // ��ѡ����ӡ����Ĵ���λ�ú�����
            printf("Error at index %d: wrote %02X, read %02X\r\n", i, writeData[i], readData[i]);
        }
    }
    
    if (errorCount == 0) {
        printf("Data Verify Success!\r\n");
    } else {
        printf("Data Verify Failed! Error Count: %d\r\n", errorCount);
    }
}






