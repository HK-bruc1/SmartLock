#include "main.h"


/**************************************************
*������    ��LCD_spi_IO_init
*��������  ��ģ��SPI�Ĺܽų�ʼ����
*��������  ����
*��������ֵ����
*��������  ��LCD_CLK----------------------------PB13     
            LCD_SDA----------------------------PB15        
****************************************************/
void LCD_spi_IO_init(void)
{
	//GPIO��ʼ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//1.��ʱ��
	
	//�������
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin =  GPIO_Pin_13 | GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Fast_Speed;
	GPIO_Init(GPIOB, &GPIO_InitStruct);	
	
	//����
	LCD_SPI_SCL_L;
}



/**************************************************
*������    ��LCD_spi_byte
*��������  ��SPI����һ���ֽں���
*��������  ��u8 data
*��������ֵ����
*��������  ��    
****************************************************/
void LCD_spi_byte(u8 data)
{
	u8 i;

	/*��������*/
	for(i=0;i<8;i++)
	{
		LCD_SPI_SCL_L;        //����ʱ���ߣ���������д����   					
		
		if(data & 0x80) LCD_MOSI_H; 
		else LCD_MOSI_L;
		
		LCD_SPI_SCL_H;       //���������أ������ӻ���
		
		data = data << 1;     //������һΪ����
	}	

}



/**************************************************
*������    ��LCD_IO_init
*��������  ��LCD�Ĺܽų�ʼ����
*��������  ����
*��������ֵ����
*��������  �� LEDK-----PB1        ���������        //ͨ�����
            RESET----PB10        ��λ����          //ͨ�����
            LCD_CS---PB12        Ƭѡ����          //ͨ�����	
            LCD_D/C--PB14        ��������ѡ����    //ͨ�����		  
****************************************************/
void LCD_IO_init(void)
{
	//GPIO��ʼ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//1.��ʱ��
	
	//�������
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin =  GPIO_Pin_1 | GPIO_Pin_10 | GPIO_Pin_12 | GPIO_Pin_14;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStruct);	
	
	//��ʼ״̬
	LCD_LED_OFF;               //�����
	LCD_CS_H;                  //Ƭѡ����
}


/**************************************************
*������    ��st7789vm_rest
*��������  ����LCD��λ����
*��������  ����
*��������ֵ����
*��������  ��  
****************************************************/
void st7789vm_rest(void)
{
	LCD_RES_L;
	tim5Delay_Ms(100);
	LCD_RES_H;
	
}


/**************************************************
*������    ��st7789vm_write_command
*��������  ����LCDд�����
*��������  ��u8 cmd
*��������ֵ����
*��������  ��    
****************************************************/
void st7789vm_write_command(u8 cmd)
{
	LCD_CS_L;//������Ƭѡ
	LCD_CMD;//��ʾ���͵�������
	LCD_spi_byte(cmd);//���巢�͵�8λ ����
	LCD_CS_H;//����Ƭѡ
}



/**************************************************
*������    ��st7789vm_write_command_parameter
*��������  ����LCDд�������ֵ����
*��������  ��u8 cmd
*��������ֵ����
*��������  ��    
****************************************************/
void st7789vm_write_command_parameter(u8 cmd_data)
{
	LCD_CS_L;//������Ƭѡ
	LCD_DATA;//��ʾ���͵�������
	LCD_spi_byte(cmd_data);//���巢�͵�8λ �������
	
	LCD_CS_H;//����Ƭѡ
}



/**************************************************
*������    ��st7789vm_write_data
*��������  ����LCDд���ݺ���
*��������  ��u16 data
*��������ֵ����
*��������  ������ֵ����ɫֵ   
****************************************************/
void st7789vm_write_data(u16 data)
{
	LCD_CS_L;//������Ƭѡ
	LCD_DATA;//��ʾ���͵�������
	LCD_spi_byte(data>>8);//���巢�͵�8λ �������,�߰�λ�ٷ��Ͱ�λ
	LCD_spi_byte(data);
	LCD_CS_H;//����Ƭѡ
}



/**************************************************
*������    ��LCD_init
*��������  ����LCD��ʼ����
*��������  ����
*��������ֵ����
*��������  ��  
****************************************************/
void LCD_init(void)
{
	//SPI_IO��ʼ��
	LCD_spi_IO_init();
	//LCD_IO��ʼ��
	LCD_IO_init();
	//LCD��λ
	st7789vm_rest();
	
	//IC����������ֲ
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
	
	//������
	LCD_LED_ON;
	//����
    //LCD_clear(RED);
}




/**************************************************
*������    ��LCD_clear
*��������  ������
*��������  ��u16 color
*��������ֵ����
*��������  ��  
****************************************************/
void LCD_clear(u16 color)
{
	/*ȷ������*/
	//ȷ��x����
	st7789vm_write_command(0x2A);     //����������
	//��ʼ������
	 st7789vm_write_data(0);
	//����������
	 st7789vm_write_data(LCD_W-1);
	
	//ȷ��Y����
	st7789vm_write_command(0x2B);     //����������
	//��ʼ������
	st7789vm_write_data(0);
	//����������
	st7789vm_write_data(LCD_H-1);
	
	
	/*ȷ����ɫ*/
	st7789vm_write_command(0x2C);     //��ɫ����
	for(u32 i=0;i<LCD_W*LCD_H;i++)
	{
		st7789vm_write_data(color);
	}
}




















