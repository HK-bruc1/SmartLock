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
	//������������ƽ���ʱ���д�ӡͼƬ������ˢ������������ˢ�����ڴ�ӡͼƬ
    //LCD_clear(BLACK);
}




/**************************************************
*������    ��LCD_clear
*��������  ������
*��������  ��u16 color
*��������ֵ����
*��������  ���������������ȷ��һ�����������λ�ã��ٰ������ڵ���ɫֵȫ�����Ϊͬһ����ɫֵ�����൱�ڰ������ڵ���ɫȫ�������
****************************************************/
void LCD_clear(u16 color)
{
	/*ȷ������*/
		//ȷ��x���򣬴�0~239��240�����ص�û����
		st7789vm_write_command(0x2A);     //����������
		//��ʼ������
		st7789vm_write_data(0);
		//���������꣬ӳ�������ֵ
		st7789vm_write_data(LCD_W-1);
		
		//ȷ��Y����
		st7789vm_write_command(0x2B);     //����������
		//��ʼ������
		st7789vm_write_data(0);
		//����������
		st7789vm_write_data(LCD_H-1);
	
	
	/*ȷ����ɫ*/
	//��Ļ�Ƕ�ά������ʽ�������������������ж�Ӧλ�õ���ɫ��Ϣ��һά������ʽ���Դ洢�ģ���������ѭ���������
	st7789vm_write_command(0x2C);     //��ɫ����
	//����������ص�ĸ�����Ҳû�д�240*240
	for(u32 i=0;i<LCD_W*LCD_H;i++)
	{
		st7789vm_write_data(color);
	}
}


/**************************************************
*������    ��LCD_xy_clear
*��������  ����Ļĳ������Ϊĳ����ɫ
*��������  ��u16 x,u16 y,u16 w,u16 h,u16 color,u8 mode,u8 *pic
*��������ֵ����
*��������  �� mode 0:�Զ�����ɫ
			 mode 1:ͼƬ��ɫ
****************************************************/
void LCD_xy_clear(u16 x,u16 y,u16 w,u16 h,u16 color,u8 mode,u8 *pic){
	u16 *p;
	u16 a = 0;

	//������Ҫ����Ŀ�ʼλ�õ����ص���ͼƬ��ģ�����е�λ��
	//��ƫ�Ƶ����ص���ɫֵ����ʼλ�ü�����ͷ����Ϣ����ƫ�Ƶ�ָ��λ��
	//240*240�����ص㣬ÿһ��240�����ص㣬��������ƫ��y�У���ƫ��x�����ص㣬�ٳ���2����Ϊһ�����ص��������ֽ�
	//�Ϳ���ƫ�Ƶ�ָ��λ�õ�ͼƬģ����λ�õ���ʼλ�ã��ٸ���ȷ�����������������ɫֵ���õ���u16һ�����ص����ɫֵ��
	p = (u16 *)(pic+sizeof(HEADCOLOR)+(y*240+x)*2);

	/*ȷ������*/
		//ȷ��x����
		st7789vm_write_command(0x2A);     //����������
		//��ʼ�������ֵ
		st7789vm_write_data(x);
		//�����������ֵ��w=20,x=10,����x+w=30,��������ʼ��������ظ�������-1��10~29�պ�20�����ص�
		st7789vm_write_data(x+w-1);
		
		//ȷ��Y����
		st7789vm_write_command(0x2B);     //����������
		//��ʼ������
		st7789vm_write_data(y);
		//����������
		st7789vm_write_data(y+h-1);
	
	
	/*ȷ����ɫ*/
	//��Ļ�Ƕ�ά������ʽ�������������������ж�Ӧλ�õ���ɫ��Ϣ��һά������ʽ���Դ洢�ģ���������ѭ���������
	//�������õ������С��ѭ�������ɫ
	//���������򣬶������ɫֵҲ��Ч��u32 i=0;i<LCD_W*LCD_H;i++
	//�������õı߽���Զ����У��������Ҳ����Ӱ������λ��
	st7789vm_write_command(0x2C);     //��ɫ����
	//������Ҫ�����ٴΣ��Ǿ�Ҫ�����ܼ��ж��ٸ����ص��ˣ�x�����ж��ٸ����ص㣬y�����ж��ٸ����ص�
	//��w=20,h=20,��ô����20*20�����ص㣬����w*h,��0��ʼ������i<w*h
	//��������λ�ö�����һ�����ص��
	for(u32 i = 0;i<w*h;i++){
		if(mode==0){
			//�Զ�����ɫ
			st7789vm_write_data(color);
		}
		else if(mode==1){
			//ͼƬ����ɫ��ͨ��ָ���õ���Ӧһ�����ص���ɫֵд��
			st7789vm_write_data(*p);
			//ƫ�Ƶ���һ�����ص��λ��
			p++;
			//��¼��ָ����������˶��ٸ����ص㣬�������w,˵��ָ���������һ���Ѿ��������
			//�ͻ��У�����ָ���������һ�п�ʼ���
			a++;
			if(a==w){
				//һ������������ָ����������ص����ɫֵ�������ǰ�治��ָ���Ķ�������
				p+=(LCD_W-w);
				//һ�����ؼ�¼����
				a=0;
			}
		}
	}
}



/***************************************************
*������    ��LCD_Ring
*��������  ��LCD��Ļ��Բ����
*��������  ��u16 x,u16 y,u16 r,u16 colour 
*��������ֵ����
*��������  ����x - x0��^2 + (y - y0)^2  == r*r 
*�������ԣ�Բ�ܾ���Բ�ĵľ�����ͬ������ͨ��ͨ��Բ�ļӰ뾶�ó��������α������п��ܵĵ�
*��� r? �� d? �� (r+2)? ���������Բ���ϣ�����γ���һ��2���ؿ��Բ����
*����Բ�뾶�õ���������������������ҵ���������Բ�ϵ����������ص�
*****************************************************/
void LCD_Ring(u16 x,u16 y,u16 r,u16 colour)
{
	u16 i,j;
	for(i=x-r-2;i<=x+r+2;i++)
	{
		for(j=y-r-2;j<=y+r+2;j++)
		{
			if((i-x)*(i-x)+(j-y)*(j-y) >= r*r && (i-x)*(i-x)+(j-y)*(j-y)<=(r+2)*(r+2))
			{
				LCD_Point(i,j,colour);
			}
		}
	}
}


/***************************************************
*������    ��LCD_shiRing
*��������  ��LCD��Ļ��Բ����
*��������  ��u16 x,u16 y,u16 r,u16 colour 
*��������ֵ����
*��������  ����x - x0��^2 + (y - y0)^2  == r*r 
*������С��Բ�뾶�������α������п��ܵĵ㶼���
*****************************************************/
void LCD_shiRing(u16 x,u16 y,u16 r,u16 colour)
{
	u16 i,j;
	for(i=x-r;i<=x+r;i++)
	{
		for(j=y-r;j<=y+r;j++)
		{
			if((i-x)*(i-x)+(j-y)*(j-y) <= r*r)
			{
				LCD_Point(i,j,colour);
			}
		}
	}
}


/**************************************************
*������    ��LCD_Point
*��������  ����㺯��
*��������  ��u16 x,u16 y,u16 color
*��������ֵ����
*��������  ��  ��׼��һ�����ص�λ���ϣ�Ҳ����x�Ŀ�ʼ�ͽ���������ͬ��ֵ��y�Ŀ�ʼ�ͽ���Ҳ����ͬ��ֵ
****************************************************/
void LCD_Point(u16 x,u16 y,u16 color)
{
	/*ȷ������*/
	//ȷ��x����
	st7789vm_write_command(0x2A);     //����������
	//��ʼ������
	 st7789vm_write_data(x);
	//����������
	 st7789vm_write_data(x);
	
	//ȷ��Y����
	st7789vm_write_command(0x2B);     //����������
	//��ʼ������
	st7789vm_write_data(y);
	//����������
	st7789vm_write_data(y);
	
	
	/*ȷ����ɫ*/
	st7789vm_write_command(0x2C);     //��ɫ����
	st7789vm_write_data(color);

}


/*
Ŀ���ļ�:C:\Users\admin\Desktop\ZK\ZK.bin   ��С:0x00141F58
�����ִ�С����:

����                        �ļ���                                          ��С                   ƫ����            ǿ�Ʋ���
 1      C:\Users\admin\Desktop\ZK\ASC16.bin            	          			0x00000806        0x00000000             ��
 2      C:\Users\admin\Desktop\ZK\ASC32.bin                                 0x00002006        0x00000806             ��
 3      C:\Users\admin\Desktop\ZK\HZK16.bin                                 0x0003FE46        0x0000280C             ��
 4      C:\Users\admin\Desktop\ZK\HZK32.bin                                 0x000FF906        0x00042652             ��
*/
/******************************************************************************
*������    ��LCD_dis_ch
*��������  ��LCD��Ļ��ʾ���趨��С���ַ�
*��������  ��u16 x,u16 y,u8 ch,u16 color,u8 size,u8 mode,u16 b_color
*��������ֵ����
*��������  ��mode ����0   �ޱ���ɫ  ����ɫ������㴫
            mode ����1   �б���ɫ  ����ɫ�������������趨
			size  ����16  16*16
			size  ����32  32*32
********************************************************************************/
void LCD_dis_ch(u16 x,u16 y,u8 ch,u16 color,u8 mode,u16 b_color,u8 size){
	u8 n,i,j;
	u16 temp;
	u8 ch16[16];
	u8 ch32[64];
	//�����ַ����һ���洢�ַ���ƫ�������ֿ��0��ʼ���ɵ��ļ���û��32��ʼ��
	n = ch - 0;
	//����ƫ������w25q64���ó���Ӧ���ݣ������ֿ��ļ���Ϊ��w25q64�л�ȡģ����
	if(size==16){
		//һ���ַ���ģ������16���ֽ�
		w25q64_read(ZK_ADDR+0+n*16,16,ch16);
	}else if(size==32){
		//һ���ַ���ģ������64���ֽ�
		w25q64_read(ZK_ADDR+0x00000806+n*64,64,ch32);
	}


	//�ַ���ʾ�ռ䣺������ʾ��Ҫ32�У�ÿһ��16�����ص���Ҫ��������ģ����Ԫ�����ж��Ƿ���
	//ģ�����������ֽ����ݱ�ʾһ���Ƿ���Ҫ���
	for(i=0;i<size;i++){
		//ȡ��һ�����ص��ģ����
		if(size==16){
			//ƫ��һ���ַ����൱��ģ��������Ԫ��ƫ��16��Ԫ�أ�16*16��ʵ�����ֿ���8������16
			temp = ch16[i];
		}else if(size==32){
			//ƫ��һ���ַ����൱��ģ��������Ԫ��ƫ��64��Ԫ�ء�32*32,�ֿ���16������32
			temp = ch32[2*i]<<8 | ch32[2*i+1];
		}

		//ÿ�е����ص����⣬���忴ÿһ�е����ص��ж��ٸ����ص㣬�Ӹ�λ����λ�ж�
		for(j=0;j<(size/2);j++){
			if(temp & (1<<(size/2-1-j))){
				LCD_Point(x+j,y+i,color);//������ɫ
			}
			else{
				if(mode == 1){
					//����Ŀ���ʹ򱳾�ɫ��ʵ�ָ��Ǿɵ���ɫ����,����Ҫ������ʵ�־ֲ�ˢ��
					LCD_Point(x+j,y+i,b_color);//����ɫ
				}
			}
		}
	}
}



/******************************************************************************
*������    ��LCD_dis_hz
*��������  ��LCD��Ļ��ʾ���趨��С���ַ�
*��������  ��u16 x,u16 y,u8 *hz,u16 color,u8 size,u8 mode,u16 b_color
*��������ֵ����
*��������  ��mode ����0   �ޱ���ɫ  ����ɫ������㴫
			mode ����1   �б���ɫ  ����ɫ�������������趨
			size  ����16  16*16
			size  ����32  32*32
			������Ҫ�����ֽڣ��൱�ڴ��ַ���,��һ����ַ��
********************************************************************************/
void LCD_dis_hz(u16 x,u16 y,u8 *hz,u16 color,u8 mode,u16 b_color,u8 size){
    u16 n = 0;//���еĺ���ƫ�����ܴ�
	u8 i,j;
    u32 temp;
	u8 hz16[32];
	u8 hz32[128];

    
	// ����Ҫ��ʾ�ĺ������һ�����ֵ�ƫ����������ģ���ݵĿ�ʼλ�ã�GB2312���ַ������λ�룬��PDF��ÿһ������94����û�м���ͷ��β
	//��ƫ�ƣ�����˼����������˱������ٸ�����
	n = (*hz-0xa1)*94 + (*(hz+1)-0xa1);//�����ֽڣ�ǰ�벿�������룬��벿����λ��


	//��w25q64�л�ȡ��Ӧ���ֵ�ģ����
	if(size==16){
		w25q64_read(ZK_ADDR+0x0000280C+n*32,32,hz16);
	}else if(size==32){
		w25q64_read(ZK_ADDR+0x00042652+n*128,128,hz32);
	}




    
    //����ģ�����жϵ����Ƿ���
	for(i=0;i<size;i++){
		// 16*16�������ʾ
		if(size==16){
			// ÿ�������������ֽ�ģ��������Ƿ��㣬ʵ�ʸ߿���16*16������ģ��������Ԫ��ƫ��32��Ԫ��
			//ÿһ�����ص��ж�ʱ��ȡ16λҲ��������u8Ԫ��
			//��һ�����ֵ�ģ����Ϊ0��1�±�Ԫ�أ��ڶ������ֵ�ģ����Ϊ2��3�±�Ԫ�أ��Դ�����
			temp = (hz16[i*2]<<8) | hz16[i*2+1];
		}
		// 32*32�������ʾ
		if(size==32){
			// ÿ���������ĸ��ֽ�ģ��������Ƿ���
			//��һ�����ֵ�ģ����Ϊ0��1��2��3�±�Ԫ�أ��ڶ������ֵ�ģ����Ϊ4��5��6��7�±�Ԫ�أ��Դ�����
            temp = (hz32[i*4]<<24) | (hz32[i*4+1]<<16) | (hz32[i*4+2]<<8) | (hz32[i*4+3]);
		}
		
		//ÿһ��������ص��ж�
		for(j=0;j<size;j++){
			if(temp & (1<<(size-1-j))){  // �Ӹ�λ��ʼ�ж�
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
*������    ��LCD_dis
*��������  ��LCD��Ļ��ʾ��ѡ��С�ĺ��ֺ��ַ����
*��������  ��u16 x,u16 y,u16 color��u8 *str��u8 size ,u8 mode,u16 b_color 
*��������ֵ����
*��������  ��
			size �ֺŴ�С   16  32
			mode ����1  �ʹ�������ɫ
			mode ����0  �Ͳ���������ɫ
********************************************************************************/
void LCD_dis(u16 x,u16 y,u8 *str,u16 color,u8 mode,u16 b_color,u8 size){
	//���������ַ���
	while(*str != '\0'){
		//�ж����ַ�
		//��ַ�д�����ַ����ߺ��ֵı���ֵ
		if(*str>=32 && *str<=127){
			//�ж�λ���Ƿ������ʾ,�ַ�ʵ��ռ�ÿ��Ϊsize/2
			//240-16/2=232����x����232ʱ��ʱ�򣬾���Ҫ���У�����һ���ַ����
			if(x > (LCD_W-size/2)){
				x=0;
				y+=size;//��һ�У��ַ�ʵ��ռ�ø߶�Ϊsize
			}
			//ȡֵ�����ַ�
			LCD_dis_ch(x,y,*str,color,mode,b_color,size);
			str++;//��һ��λ��ƫ��һ���ֽڣ��ַ��Ļ�ƫ��һ���ֽڣ�ASCII����u8��
			x+=size/2;//һ���ַ��Ŀ��Ϊsize/2
		}
		else{
			//�����ַ����Ǻ���
			//�ж�λ���Ƿ������ʾ,�ַ�ʵ��ռ�ÿ��Ϊsize/2
			//240-32=208����x����208ʱ��ʱ�򣬾���Ҫ���У�����һ�����ֿ��
			if(x > (LCD_W-size)){
				x=0;
				y+=size;//��һ�У��ַ�ʵ��ռ�ø߶�Ϊsize
			}
			LCD_dis_hz(x,y,str,color,mode,b_color,size);
			str+=2;//��һ��λ��,���ֵĻ�ƫ�������ֽ�
			x+=size;//һ�����ֵĿ��Ϊsize
		}
	}
}



/**********************************************
*������    ��LCD_dis_pic
*��������  ��LCD��Ļ��ʾ�����СͼƬ
*��������  ��u16 x,u16 y,u8 *pic
*��������ֵ����
*��������  ������������һ����ֻ����ÿһ�������ɫֵ��ͼƬ����ɫֵ�����Ƕ��Ǳ���ɫ
*ͼƬȡģ�����ص����ɫֵ���ַ��ͺ���ȡģ�������ж��Ƿ���Ҫ����
************************************************/
void LCD_dis_pic(u16 x,u16 y,const u8 *pic){
	//ͼƬģ����ͷ����Ϣ��˵����ȡģ�����
	HEADCOLOR *head;
	//ÿһ�����ص���Ҫ�����ֽ���������ʾ��ɫֵ�����Զ���u16,ƫ�Ʒ��㣬++ֱ��ƫ�������ֽ�
	u16 *p;

	//��ȡͷ����Ϣ,ǿ������ת�����õ���Ӧͷ����Ϣ
	head = (HEADCOLOR *)pic;
	//�õ�ͼƬ���ص���ɫ���ݣ�ȥ��ͷ����Ϣ���µľ������ص���ɫ����
	p = (u16*)(pic+sizeof(HEADCOLOR));


	/*ȷ������*/
	//ȷ��x����
	st7789vm_write_command(0x2A);     //����������
	//��ʼ������
	st7789vm_write_data(x);
	//����������
	//��������240�Ļ�����ôx~x+240-1�պ���240�����ص�
	st7789vm_write_data(x+head->w-1);
	
	//ȷ��Y����
	st7789vm_write_command(0x2B);     //����������
	//��ʼ������
	st7789vm_write_data(y);
	//����������
	st7789vm_write_data(y+head->h-1);
	
	
	/*ȷ����ɫ*/
	st7789vm_write_command(0x2C);     //��ɫ����
	//�����ص������������Ҫ������ɫ����
	//����240*240�����ص�
	for(u32 i=0;i<head->w*head->h;i++){
		//u16,��һ�����ص���������
		st7789vm_write_data(*p);
		//ƫ�Ƶ���һ�����ص���ʼ��ַ
		p++;
	}

}



/**********************************************
*������    ��LCD_dis_number_val
*��������  ����ʾ�����ַ���
*��������  ��u16 x,u16 y,u8 *numstr,u16 color,u8 mode,u16 b_color
*��������ֵ����
*��������  ��mode ����1  �ʹ�������ɫ
			mode ����0  �Ͳ���������ɫ
************************************************/
void LCD_dis_number_val(u16 x,u16 y,u8 *numstr,u16 color,u8 mode,u16 b_color)
{
	u16 n,i,j;
	//48*48 �������ʾ��ȡ����һ��48���ص��Ƿ����ģ����Ԫ�ظ�����6����6*8=48�պ��ж�һ��ÿһ�����ص��Ƿ���
	//8���ֽڵ��������͸ոպ�
	long long temp;
	
	
	//���������ַ��������ʾ
	while(*numstr!='\0')
	{
		//ÿ��һ�������ַ�����ʾ��n��Ϊѭ�����ܴӵ�һ���ַ����鿪ʼ�ж�ƫ����
		n = 0;
		/*�����ÿһ��Ҫ��ʾ�����������һ�������ַ�ƫ������const u8 numb_table[] = "0123456789:Z";*/
		while(numb_table[n] != '\0')
		{
			if(*numstr == numb_table[n])
			{
				//�ӵ�һ����ʼ�ң��ҵ��ͽ�������ַ�ƫ�Ƶļ��㡣n ���Ƕ�Ӧ�ַ���ƫ����
				break;
			}
			n++;
		}

		//n������β0���������ˣ�ȫ�������ˣ�û�ж�Ӧ�ַ�����Ȼ��û��ģ��������
		if(numb_table[n] == '\0')
		{
			return;
		}
		
		
		//һ��������ѭ����48��
		for(i=0;i<48;i++)
		{
			//ȡ��һ�е����ص��ģ����Ԫ�أ�48�����ص㣬��Ҫȡ��6��ģ����Ԫ�شճ�һ���Ƿ�����ж�
			//0 1 2 3 4 5   �ڶ��Σ�6 7 8 9 10 11��������
			//һ���ַ�48*48����Ӧ��ģ����Ԫ�ظ�����288����һ����Ҫ6��ģ����Ԫ�أ�6*48=288
			temp = (long long)number48[n*288+6*i]<<40 | (long long)number48[n*288+6*i+1]<<32 | (long long)number48[n*288+6*i+2]<<24 | (long long)number48[n*288+6*i+3]<<16 | (long long)number48[n*288+6*i+4]<<8 | (long long)number48[n*288+6*i+5];
			//ÿ�е����ص����ѭ�����õ�һ���Ƿ����ģ���ݺ���λ�ж�
			for(j=0;j<48;j++)
			{
				//�ж�Ҫ�������ص㣬�Ӹ�λ����λ����ж�
				if(temp & ((long long)1<<(47-j)))
				{
					LCD_Point(x+j,y+i,color);
				}
				else
				{
					//���������ص����ɫ
					if(mode == 1)
					{
						LCD_Point(x+j,y+i,b_color);
					}
				}
			}
		}
		
		//�����ж���һ�������ַ���
		numstr++;
		//ƫ�Ƶ���һ���ַ���ʾ��λ��
		x+=48;
	}
}





/**********************************************
*������    ��LCD_dis_number_pic
*��������  ����ͼƬ����ʾ�����ַ��� 
*��������  ��u16 x,u16 y,u8 *numstr,u16 color,u8 mode,const u8 *pic
*��������ֵ����
*��������  ��mode ����1  �ʹ�������ɫ
						mode ����0  �Ͳ���������ɫ
************************************************/
void LCD_dis_number_pic(u16 x,u16 y,u8 *numstr,u16 color,u8 mode,const u8 *pic)
{
	u16 n,i,j;
	long long temp;
	//һ�����ص����ɫֵռ�����ֽ����ݣ���Ҫһ���Դ�ģ������ȡ������Ԫ��
	u16 *b_pic;
	
	
	while(*numstr!='\0')
	{
		n = 0;
		/*�����Ҫ��ʾ�����������һ�������ַ�ƫ����*/
		while(numb_table[n] != '\0')
		{
			if(*numstr == numb_table[n])
			{
				break;
			}
			n++;
		}
		if(numb_table[n] == '\0')
		{
			return;
		}
		
		
		//һ��������ѭ��
		for(i=0;i<48;i++)
		{
			temp = (long long)number48[n*288+6*i]<<40 | (long long)number48[n*288+6*i+1]<<32 | (long long)number48[n*288+6*i+2]<<24 | (long long)number48[n*288+6*i+3]<<16 | (long long)number48[n*288+6*i+4]<<8 | (long long)number48[n*288+6*i+5];
			//ÿ�е����ص����ѭ��
			for(j=0;j<48;j++)
			{
				//�ж�Ҫ�������ص�
				if(temp & ((long long)1<<(47-j)))
				{
					LCD_Point(x+j,y+i,color);
				}
				else
				{
					if(mode == 1)
					{
						//����ɫ����Ӧλ�õ���ɫֵ���ҵ���Ӧλ�����ص����ɫֵ
						//������ͼƬģ�����ͷ����Ϣ���ٸ������ص������ƫ�����ҵ���Ӧ���ص����ɫֵ 
						//����Y������ص�ƫ����������X������ص�ƫ�����������������2����Ϊһ�����ص����ɫֵռ�����ֽ�����
						b_pic = (u16 *)(pic+sizeof(HEADCOLOR)+((y+i)*LCD_W+(x+j))*2);
						LCD_Point(x+j,y+i,*b_pic);
					}
				}
			}
		}
		
		numstr++;
		x+=48;
	}
}






