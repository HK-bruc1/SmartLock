#include "main.h"


//ÿһ��ʹ��IICͨ�ŵ��������в�ͬ��Ҫ��

/***********************************************
*������    :bs8116_iic_IO_init
*��������  :IIC����IO�ڳ�ʼ������
*��������  :��
*��������ֵ:��
*��������  : PB6----SCL   ͨ�ÿ�©���
            PB7----SDA   ͨ�ÿ�©���
************************************************/
void bs8116_iic_IO_init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//�˿�ʱ��ʹ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;          //ͨ�����
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;         //��©
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 ;  // 6    7
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;       //��������
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;       //�ٶ�
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	
	//����״̬
	GPIO_SetBits(GPIOB, GPIO_Pin_6);//SCL
	GPIO_SetBits(GPIOB, GPIO_Pin_7);//SDA
	
	
}




/***********************************************
*������    :iic_star
*��������  :IIC��ʼ�źź���
*��������  :��
*��������ֵ:��
*��������  :ʱ����Ϊ�ߵ�ƽ�������߳����½���
************************************************/
void bs8116_iic_star(void)
{
	//ʱ�������ͣ��ſ��Զ�������
	BS8116_IIC_SCL_L; 
	tim5Delay_Us(5);
	//���������ߣ�Ŀ����Ϊ�����½�����׼��
	BS8116_IIC_SDA_OUT_H;   
	//ʱ��������  ������ʼ�źŵ�����
	BS8116_IIC_SCL_H;
	tim5Delay_Us(5);
	//����������  ���Բ�����ʼ�ź�
	BS8116_IIC_SDA_OUT_L;
	tim5Delay_Us(5);
	
	//��ȫ
	BS8116_IIC_SCL_L;
	
}




/***********************************************
*������    :iic_stop
*��������  :IICֹͣ�źź���
*��������  :��
*��������ֵ:��
*��������  :ʱ����Ϊ�ߵ�ƽ�������߳���������
************************************************/
void bs8116_iic_stop(void)
{
	
	//׼������
	BS8116_IIC_SCL_L;
	BS8116_IIC_SDA_OUT_L;
	
	//����ֹͣ�ź�
	BS8116_IIC_SCL_H;
	tim5Delay_Us(5);
	BS8116_IIC_SDA_OUT_H;
	tim5Delay_Us(5);

}

/***********************************************
*������    :iic_send_ack
*��������  :����Ӧ��/��Ӧ���ź�
*��������  :u8 ack
*��������ֵ:��
*��������  :Ӧ�� ʱ����Ϊ�ߵ�ƽ���������Ѿ����ֺõ͵�ƽ��
						��Ӧ�� ʱ����Ϊ�ߵ�ƽ���������Ѿ����ֺøߵ�ƽ��				
						������0   ����Ӧ���ź�
						������1   ���Ͳ�Ӧ���ź�
************************************************/
void bs8116_iic_send_ack(u8 ack)
{
		BS8116_IIC_SCL_L;      //ʱ�������ͣ��ſ��Զ�������
		tim5Delay_Us(3);
		if(ack == 0)       //����Ӧ��
		{
			BS8116_IIC_SDA_OUT_L;  //���������ȱ��ֵ͵�ƽ
		}
		else if(ack == 1)
		{
			BS8116_IIC_SDA_OUT_H;  //���������ȱ��ָߵ�ƽ
		}
		tim5Delay_Us(2);
		BS8116_IIC_SCL_H;      //������Ӧ���ź�	
		tim5Delay_Us(5);
	
	
	//��ȫ
	BS8116_IIC_SCL_L;
	
}



/***********************************************
*������    :iic_get_ack
*��������  :���Ӧ��/��Ӧ���ź�
*��������  :��
*��������ֵ:u8
*��������  :
						����0 ��ʾӦ���ź�
						����1 ��ʾ��Ӧ���ź�
************************************************/
u8 bs8116_iic_get_ack(void)
{
	
	u8 ack = 0;
	
	//������ҪΪ����
	BS8116_IIC_SCL_L;
	//�������л�����״̬
	BS8116_IIC_SDA_OUT_H;    
	
	//���Ӧ��/��Ӧ���ź�
	BS8116_IIC_SCL_L;   //���������ӻ�����ʱ���ߣ��ӻ��ſ��Զ������ߣ�������Ӧ��/��Ӧ���ź�
	tim5Delay_Us(5);  
	BS8116_IIC_SCL_H;   //�����ſ��Զ�������
	tim5Delay_Us(5);
	if(BS8116_IIC_SDA_INT == 0)
	{
		//�͵�ƽ����Ӧ���ź�
		ack = 0;
	}
	else
	{
		//�ߵ�ƽ----��Ӧ���ź�
		ack = 1;
	}
	
	//��ȫ��BS8116����Ҫ�󣬴�ʱ�ӻ������ͣ�����Ҫ����������
	//BS8116_IIC_SCL_L;
	return ack;
	
}


/***********************************************
*������    :iic_send_byte
*��������  :IIC����һ���ֽں���
*��������  :u8 data
*��������ֵ:��
*��������  :
************************************************/
void bs8116_iic_send_byte(u8 data)
{
	u8 i;
	for(i=0;i<8;i++)
	{
		//ʱ�������Ͳſ��Ըı�������
		BS8116_IIC_SCL_L;
		tim5Delay_Us(3);  
		//���ݲ����Ķ�Ӧλ��0����1���������ߵĸߵ�
		if(data & 0x80)
		{
			BS8116_IIC_SDA_OUT_H;
		}
		else
		{
			BS8116_IIC_SDA_OUT_L;
		}
		tim5Delay_Us(2);  
		//ʱ�������ߣ������ӻ��������ӻ��ſ��Զ���һλ����
		BS8116_IIC_SCL_H;
		tim5Delay_Us(5);  
		//��һλ���ݣ��θ�λ������λ
		data = data << 1;
	}
	//��ȫ
	BS8116_IIC_SCL_L;
	
}


/*******************************************
*������    :iic_read_byte
*��������  :����ͨ��IIC����һ���ֽں���
*��������  :��
*��������ֵ:u8
*��������  :
*********************************************/
u8 bs8116_iic_read_byte(void)
{
	
	u8 data;    
	u8 i;
	
	//��IO���л�Ϊ����
	BS8116_IIC_SCL_L;
	BS8116_IIC_SDA_OUT_H;
	
	//������
	for(i=0;i<8;i++)
	{
		BS8116_IIC_SCL_L;             //���������ӻ�����ʱ����
		tim5Delay_Us(5);   
		BS8116_IIC_SCL_H;             //�������Զ�����
		tim5Delay_Us(5);
		data = data << 1;
		if(BS8116_IIC_SDA_INT) 
			data= data | 0x01;	
		
	}
	
	//��ȫ
	BS8116_IIC_SCL_L;
	
	return data;
	
}






/***********************************************
*������    :BS8116_init
*��������  :BS8116��ʼ������
*��������  :��
*��������ֵ:��
*��������  :IRQ-----PA1
************************************************/
void BS8116_init(void)
{
	
	u8 register_buff[21] = {0};
	
	//BS8116ģ��ĳ�ʼ����
	u8 write_buff[21] = {	0x00,0x00,0x83,0xf3,0x98,
							0x3f,0x3f,0x3f,0x3f,0x3f,
							0x3f,0x3f,0x3f,0x3f,0x3f,
							0x3f,0x3f,0x3f,0x3f,0x3f,0x7f};	
	
	/*IIC����IO�ڳ�ʼ��*/
	bs8116_iic_IO_init();

	/*IRQ����IO�ڳ�ʼ��,����Ĭ�ϣ�����Ƿ��°�����*/
	RCC->AHB1ENR |=0x01<<0;//�˿�ʱ��ʹ��
	GPIOA->MODER &= ~(3<<2);
											
	


	/*�Ĵ�����ʼ����,������ǰ״̬��������Ҫ��*/									
	BS8116_Writeregister(write_buff);
	
												
	//��ȡ�Ĵ�����ֵ(�鿴���õļĴ�����ֵ),˯��״̬�²���������ò�������Ҫ�Ȼ����ٸ�λ
	// BS8116_Readregister(register_buff);
	// for(u8 i=0;i<21;i++){
	// 	printf("0x%x\r\n",register_buff[i]);
	// }

	
	
}



/***********************************************
*������    :BS8116_Writeregister
*��������  :BS8116д�Ĵ�����ֵ
*��������  :u8 *cmd_buff
*��������ֵ:u8      //ack
*��������  :
************************************************/
u8 BS8116_Writeregister(u8 *cmd_buff)
{
	//��������У���
	u32 check_sum = 0;
	
	//��ʼ�źŲ�����������ַ
	bs8116_iic_star();
	bs8116_iic_send_byte(BS8116ADDR_W);  
	//���ӻ�Ӧ��
	if(bs8116_iic_get_ack())
	{
		bs8116_iic_stop( );
		return 1;
	}
	//�ȴ��ӻ���æ���ӻ��ᴦ������ӻ�������󣬻�����SCL��
	while(!BS8116_IIC_SCL_INT);
	
	//���üĴ���������
	bs8116_iic_send_byte(0xB0);    //�Ĵ�������ʼ��ַ 
	if(bs8116_iic_get_ack())
	{
		bs8116_iic_stop( );
		return 2;
	}
	//�ȴ��ӻ���æ
	while(!BS8116_IIC_SCL_INT);
	

	/*���ͼĴ�����������*/
	//����21���Ĵ������ݺ�һ���ۼӺ�����
	for(u8 i=1;i<=22;i++)
	{
		//��22�η����ۼӺ�
		if(i==22)
		{
			bs8116_iic_send_byte(check_sum & 0xff);     //����У��� �Ͱ�λ
		}
		//21֮ǰҪ���ͼĴ������ݲ����ۼ�
		else
		{
			bs8116_iic_send_byte(*cmd_buff);
			check_sum = check_sum + *cmd_buff;         //�ۼ�
			cmd_buff++;
		}
	
		//�������Ӧ��
		if(bs8116_iic_get_ack())
		{
			bs8116_iic_stop( );
			return 3;
		}
		
		//�ȴ��ӻ���æ
		while(!BS8116_IIC_SCL_INT);
	}
	
	bs8116_iic_stop( );
	
	return 0;
}




/***********************************************
*������    :BS8116_Readregister
*��������  :BS8116���Ĵ�����ֵ
*��������  :u8 *buff 
*��������ֵ:u8 
*��������  :������21���Ĵ�����ֵ�����ڼ���Ƿ����óɹ�
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
	
	//�ӻ���ʱ�����ͱ�ʾ���ڴ�������
	//������Ҫ�ȴ��ӻ���ʱ��������
	while(!BS8116_IIC_SCL_INT);
	
	
	bs8116_iic_send_byte(0xB0);     //����ʼ�Ĵ�����ַ��ʼ
	if(bs8116_iic_get_ack())
	{
		bs8116_iic_stop( );
		return 2;
	}
	//�ȴ��ӻ���æ
	while(!BS8116_IIC_SCL_INT);
	
	bs8116_iic_star();
	bs8116_iic_send_byte(BS8116ADDR_R);  
	if(bs8116_iic_get_ack())
	{
		bs8116_iic_stop( );
		return 3;
	}
	//�ȴ��ӻ���æ
	while(!BS8116_IIC_SCL_INT);
	
	//��21�����üĴ�������
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
*������    :BS8116_ReadKey
*��������  :BS8116����ֵ
*��������  :��
*��������ֵ:u8
*��������  :
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
	//�ȴ��ӻ���æ
	while(!BS8116_IIC_SCL_INT);

	bs8116_iic_send_byte(0x08);    //���ͼ�ֵ�Ĵ����ĵ�ַ
	if(bs8116_iic_get_ack())
	{
		bs8116_iic_stop( );
		return 2;
	}
	//�ȴ��ӻ���æ
	while(!BS8116_IIC_SCL_INT);
	
	bs8116_iic_star();
	bs8116_iic_send_byte(BS8116ADDR_R);
	if(bs8116_iic_get_ack())
	{
		bs8116_iic_stop( );
		return 3;
	}  
	//�ȴ��ӻ���æ
	while(!BS8116_IIC_SCL_INT);
	
	key=bs8116_iic_read_byte();   //08
	bs8116_iic_send_ack(0);        //������
	key = (bs8116_iic_read_byte()<<8) | key;//���õ���1~8�Ű��������õ���9~16�Ű���
	bs8116_iic_send_ack(1);        //����Ҫ������
	bs8116_iic_stop( );
	//keyֵ���Ǽ�ֵ�Ĵ�������������
	printf("key:0x%x\r\n",key);	

	//����ӳ���ֵ����Ϊ������̲�����ӦBS8116�ļĴ�����ֵ����������
	
	
	
	return 0xff;
}















