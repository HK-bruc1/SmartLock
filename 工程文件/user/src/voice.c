#include "main.h"




/***********************************************
*������    ��voice_init
*��������  ��������ģ��Ĺܽų�ʼhaul����
*��������  ����
*��������ֵ����
*��������  ��VOICE_BUSY------------PC4      //ͨ������
            VOICE_DATA------------PC5      //ͨ���������
***********************************************/
void voice_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	//�˿�ʱ��ʹ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;          //ͨ�����
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;         //����
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 ;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;       //��������
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;       //�ٶ�
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;          //ͨ������
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4 ;
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
	//DATA���е�
	GPIO_ResetBits(GPIOC, GPIO_Pin_5);
	
	
	//��Ҫ�ϵ�ʱ��,ģ���ϵ���Ҫʱ��
	tim5Delay_Ms(100);
	
}


/***********************************************
*������    ��voice_head
*��������  ��ͬ��ͷ
*��������  ����
*��������ֵ����
*��������  ��
***********************************************/
void voice_head(void)
{
	VOICE_DATA_H;
	tim5Delay_Ms(8);
	VOICE_DATA_L;
	tim5Delay_Ms(1);
	
}


/***********************************************
*������    ��voice_low
*��������  ������0
*��������  ����
*��������ֵ����
*��������  ��
***********************************************/
void voice_low(void)
{
	VOICE_DATA_H;
	tim5Delay_Us(500);
	VOICE_DATA_L;
	tim5Delay_Us(1500);
	
}

/***********************************************
*������    ��voice_hig
*��������  ������1
*��������  ����
*��������ֵ����
*��������  ��
***********************************************/
void voice_hig(void)
{
	VOICE_DATA_H;
	tim5Delay_Us(1500);
	VOICE_DATA_L;
	tim5Delay_Us(500);
}


void voice(u8 cmd){
    //�ȴ���һ�β������
    while(VOICE_BUSY);
    
    //ͬ��ͷ
    voice_head();
    u8 i;
    for(i=0;i<8;i++){
        if(cmd & (0x80)){
            voice_hig();
        }else{
            voice_low();
        }
        cmd<<=1;
    }
}

