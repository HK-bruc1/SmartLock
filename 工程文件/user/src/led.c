#include "led.h"
#include "main.h"

/***************************************
*������			��led_init
*��������		��LED������IO�ڳ�ʼ������
*��������		����
*��������ֵ	����
*��������		��LED3--------PB9,LED4--------PB8
***************************************/
void led_init(void){
	//��������ʱ��ʹ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	//�˿�ģʽ����
	//�������
	//������
	//����ٶ�
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_8;//û�й�ͬλ����ͻ����
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	//������ݼĴ�������Ĭ�Ϲر�
	GPIO_SetBits(GPIOB, GPIO_Pin_8 | GPIO_Pin_9);
}