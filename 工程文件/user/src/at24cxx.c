#include "main.h"                  // Device header

/***********************************************
*������    :iic_IO_init
*��������  :IIC����IO�ڳ�ʼ������
*��������  :��
*��������ֵ:��
*��������  :PA8----SCL   ͨ���������
            PC9----SDA   ͨ�ÿ�©���
************************************************/
void iic_IO_init(void)
{
	RCC->AHB1ENR |= (1<<0);
	RCC->AHB1ENR |= (1<<2);
	GPIOA->MODER &= ~(3<<16) ;
	GPIOA->MODER |= (1<<16) ;
	GPIOC->MODER &= ~(3<<18) ;
	GPIOC->MODER |= (1<<18) ;
	//PA8����
	GPIOA->OTYPER &= ~(1<<8);
	//PC9��©
	GPIOC->OTYPER |= (1<<9);
	
	//����״̬
	IIC_SCL_H;
	IIC_SDA_OUT_H;
}



/***********************************************
*������    :iic_star
*��������  :IIC��ʼ�źź���
*��������  :��
*��������ֵ:��
*��������  :ʱ����Ϊ�ߵ�ƽ�������߳����½���
************************************************/

void iic_star(void)
{
	//ʱ�������ͣ��ſ��Զ�������
	IIC_SCL_L;  
	//���������ߣ�Ŀ����Ϊ�����½�����׼��
	IIC_SDA_OUT_H;   
	//ʱ��������  ������ʼ�źŵ�����
	IIC_SCL_H;
	tim5Delay_Us(5);
	//����������  ���Բ�����ʼ�ź�
	IIC_SDA_OUT_L;
	tim5Delay_Us(5);
	
	//��ȫ
	IIC_SCL_L;
}



/***********************************************
*������    :iic_stop
*��������  :IICֹͣ�źź���
*��������  :��
*��������ֵ:��
*��������  :ʱ����Ϊ�ߵ�ƽ�������߳���������
************************************************/
void iic_stop(void)
{
	//׼������
	IIC_SCL_L;
	IIC_SDA_OUT_L;
	
	//����ֹͣ�ź�
	IIC_SCL_H;
	tim5Delay_Us(5);
	IIC_SDA_OUT_H;
	tim5Delay_Us(5);
}



/***************************************
*������			��iic_send_ack
*��������		��IIC����Ӧ��/��Ӧ���ź�
*��������		��u8 ack
*��������ֵ	����
*��������		��ack ���� 0   ��ʾ����Ӧ��
              ack ���� 1   ��ʾ���Ͳ�Ӧ��
****************************************/
void iic_send_ack(u8 ack)
{
	IIC_SCL_L;      //���Զ�������
	tim5Delay_Us(3);
	ack ? (IIC_SDA_OUT_H) : (IIC_SDA_OUT_L);
	tim5Delay_Us(2);
	
	IIC_SCL_H;      //����Ӧ��/��Ӧ���ź�
	tim5Delay_Us(5);
	IIC_SCL_L;     //��ȫ����
}


/***************************************
*������			��iic_rec_ack
*��������		��IIC���Ӧ��/��Ӧ���ź�
*��������		����
*��������ֵ	��u8
*��������		������ 0  ��ʾӦ��
              ���� 1  ��ʾ��Ӧ��
****************************************/
u8 iic_get_ack(void)
{
	u8 ack;
	
	/*���������л�Ϊ����״̬*/
	IIC_SCL_L;      //���Զ�������
	IIC_SDA_OUT_H;
	
	/*��ȡӦ���ź�*/
	IIC_SCL_L;      //���������ӻ�����ʱ���ߣ��ӻ����ܸı�����
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
	
	
	IIC_SCL_L;    //��ȫ����
	
	return ack;
	
}



/***************************************
*������			��iic_send_byte
*��������		��IIC����һ���ֽ����ݺ���
*��������		��u8 data
*��������ֵ	����
*��������		��
****************************************/
void iic_send_byte(u8 data)
{
	u8 i;
	for(i=0;i<8;i++)
	{
		IIC_SCL_L;      //���Զ�������
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
		
		IIC_SCL_H;     //�����ӻ�����ʱ���ߣ��ӻ����Զ���λ����
		tim5Delay_Us(5);
		
		//��һλ����
		data = data << 1;
	}
	
	
	IIC_SCL_L;    //��ȫ����
	
}



/***************************************
*������			��iic_rec_byte
*��������		��IIC����һ���ֽ����ݺ���
*��������		����
*��������ֵ	��u8
*��������		��
****************************************/
u8 iic_read_byte(void)
{
	u8 i;
	u8 data;
	
	/*�������߸�Ϊ����״̬*/
	IIC_SCL_L;      //���Զ�������
	IIC_SDA_OUT_H;
	
	/*����8λ����*/
	for(i=0;i<8;i++)
	{
		IIC_SCL_L;     //�����ӻ���ʱ�������ͣ��ӻ��Ϳ��Ըı�������
		tim5Delay_Us(5);
		IIC_SCL_H;     //�������Զ�����
		tim5Delay_Us(5);
		data = data << 1;
		if(IIC_SDA_INT)
		{
			data |= 0x01;
		}
	}

	IIC_SCL_L;    //��ȫ����
	
	
	return data;
	
}



/********************************************************
*������    :at24c0x_init
*��������  :AT24C0X�ĳ�ʼ��
*��������  :
*��������ֵ:
**********************************************************/
void at24c0x_init(void)
{
	iic_IO_init();
}


/********************************************************
*������    :at24c0x_write_byte
*��������  :��at24c0x��ĳ����ַ�ռ�дһ���ֽ�����
*��������  :u16 inner_addr     //�����ڲ��洢�ռ��ַ  0~511   
						u8 data           //Ҫд�������
*��������ֵ:ÿ�η��Ͷ�������Ӧ��ͨ������ֵ�жϾ����Ĵη��ͳ�����			
*��������  :�궨������־��
							AT24C0X_NO_ERR 0;   //Ӧ��  �޴���
							AT24C0X_ERR1   1;   //��Ӧ��
							AT24C0X_ERR2   2;   //��Ӧ��
							AT24C0X_ERR3   3;   //��Ӧ��
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
	
	/*�������Ե�ַ*/
	iic_send_byte(inner_addr % 256);   //��ʵ�ڲ���ַ
	ack = iic_get_ack();
	if(ack == 1)
	{
		iic_stop();
		return AT24C0X_ERR2; 
	}
	
	
	//��������
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
*������    :at24c0x_read_byte
*��������  :��at24c0x��ĳ����ַ�ռ��һ���ֽ�����
*��������  :u16 inner_addr     //�����ڲ��洢�ռ��ַ  0~511   
						u8 *data           //�������ݴ�ŵ�ַ
*��������ֵ:ÿ�η��Ͷ�������Ӧ��ͨ������ֵ�жϾ����Ĵη��ͳ�����			
*��������  :�궨������־��
							AT24C0X_NO_ERR 0;   //Ӧ��  �޴���
							AT24C0X_ERR1   1;   //��Ӧ��
							AT24C0X_ERR2   2;   //��Ӧ��
							AT24C0X_ERR3   3;   //��Ӧ��
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
	
	/*���;��Ե�ַ*/
	iic_send_byte(inner_addr&0xff);
	//inner_addr = inner_addr & 0xff;    //�ڶ����ʵ�ʾ��Ե�ַ
	//inner_addr = inner_addr % 256;    //�ڶ����ʵ�ʾ��Ե�ַ
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
*������    :AT24C0x_read_bytes
*��������  :��AT24C0x��ĳ����ַ�ռ俪ʼ����������ֽ�����
*��������  :u16 inner_addr     //�����ڲ��洢�ռ��ַ  0~511  
            u16 num            //Ҫ�����ٸ��ֽ�
						u8 *str           //���������ݴ�ŵĵ�ַ
*��������ֵ:ÿ�η��Ͷ�������Ӧ��ͨ������ֵ�жϾ����Ĵη��ͳ�����			
*��������  :�궨������־��
							AT24C0X_NO_ERR 0;   //Ӧ��  �޴���
							AT24C0X_ERR1   1;   //��Ӧ��
							AT24C0X_ERR2   2;   //��Ӧ��
							AT24C0X_ERR3   3;   //��Ӧ��
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
	
	/*���;��Ե�ַ*/
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
	
	//ѭ��������
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
*������    :AT24C0x_write_page
*��������  :��AT24C0x��ĳ����ַ�ռ俪ʼҳд
*��������  :u16 inner_addr     //�����ڲ��洢�ռ��ַ  0~511  
            u8 num            //Ҫ�����ٸ��ֽ�
						u8 *str           //Ҫд���ַ������׵�ַ
*��������ֵ:ÿ�η��Ͷ�������Ӧ��ͨ������ֵ�жϾ����Ĵη��ͳ�����			
*��������  :�궨������־��
							AT24C0X_NO_ERR 0;   //Ӧ��  �޴���
							AT24C0X_ERR1   1;   //��Ӧ��
							AT24C0X_ERR2   2;   //��Ӧ��
							AT24C0X_ERR3   3;   //��Ӧ
**********************************************************/
u8 AT24C0x_write_page(u16 inner_addr,u8 num,u8 *str)
{
	u8 ack;
	
	
	/*ҳд*/
	iic_star();
	iic_send_byte(AT24C0x_ADDR_W+inner_addr/256*2); 
	ack = iic_get_ack();
	if(ack == 1)
	{
		iic_stop();
		return AT24C0X_ERR1; 
	}	
	
	/*���;��Ե�ַ*/
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
	
	//д����
	tim5Delay_Ms(5);
	
	return AT24C0X_NO_ERR;
}


/********************************************************
*������    :AT24C0x_write_bytes
*��������  :��AT24C0X��ĳ����ַ�ռ俪ʼ����д���Կ�ҳ
*��������  :u16 inner_addr     //�����ڲ��洢�ռ��ַ  0~511  
            u16 num            //Ҫ�����ٸ��ֽ�
						u8 *str           //Ҫд���ַ������׵�ַ
						u8 AT_flag        //��1����  AT24c02    ��2����AT24c04  
*��������ֵ:ÿ�η��Ͷ�������Ӧ��ͨ������ֵ�жϾ����Ĵη��ͳ�����			
*��������  :�궨������־��
							AT24C0X_NO_ERR 0;   //Ӧ��  �޴���
							AT24C0X_ERR1   1;   //��Ӧ��
							AT24C0X_ERR2   2;   //��Ӧ��
							AT24C0X_ERR3   3;   //��Ӧ
**********************************************************/
u8 AT24C0x_write_bytes(u16 inner_addr,u16 num,u8 *str,u8 AT_flag)
{
	u8 less_num;
	
	
	while(1)
	{
		//���㱾ҳ��ʣ���ٸ��ռ����д
		less_num = 8*AT_flag - inner_addr % (8 * AT_flag);   
		
		//�ж��Ƿ���ҳ
		if(less_num >= num)       //����ҳ
		{
			//����ҳд������д��
			AT24C0x_write_page(inner_addr,num,str);  
			//�˳�ѭ���ṹ
			break;
		}
		//��ҳ
		else
		{
			//�ȵ���ҳд�����ѱ�ҳд��
			AT24C0x_write_page(inner_addr,less_num,str);  // 
			//�洢оƬ�л�����ҳ����ʼλ��λ��
			inner_addr = inner_addr + less_num;   //
			
			//�����ʣ��Ҫд�ĸ���
			num = num - less_num;                 //
			
			//�����ʣ��Ҫд�����ݵĵ�ַ
			str = str + less_num;	
		}
	}		
	return AT24C0X_NO_ERR;
}

//AT24C0XX���Ժ���
void AT24C0XX_Test(void){
    u8 writeData[300] = {0}; // ���������С��ȷ����ҳ
    u8 readData[300] = {0};  // ��ȡ���ݻ�����
    
    // ��ʼ��at24c0x_init������
    at24c0x_init();
    printf("at24c0x Test Start...\r\n");
    
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


    // д�볬��20�ֽڵ����ݣ����Կ�ҳ
    AT24C0x_write_bytes(0,200,writeData,AT24C04);
    printf("Write 200 Bytes Data\r\n");
    
    // ��ȡ����
    AT24C0x_read_bytes(0,200,readData);
    printf("Read 200 Bytes Data\r\n");
    
    // ��ӡreadData�����ǰ16��Ԫ��
    printf("ReadData: ");
    for (int i = 0; i < 16; i++) {
        printf("%02X ", readData[i]);
    }
    printf("\r\n");
    
    // �Ƚ�д��Ͷ�ȡ������
    int errorCount = 0;
    for (int i = 0; i < 200; i++) {
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

























