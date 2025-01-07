#include "main.h"
#include "stm32f4xx.h"                  // Device header


/***************************************
*������		��key_init
*��������	���������ܳ�ʼ��
*��������	����
*��������ֵ	����
*��������	��key1-----PA0,key2-----PC13��key2����ʱio��Ϊ�͵�ƽ��key1����ʱioΪ�ߵ�ƽ
***************************************/
void key_init(void){
    //GPUO�ڵĳ�ʼ��
    //��Ӧ����ʱ��ʹ��
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    //ͨ������
    //������
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    //PA0
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    //PC13,�޸ı�Ҫ��������ͬ
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
    GPIO_Init(GPIOC, &GPIO_InitStruct);
}

/***************************************
*������		��key_scan
*��������	������ɨ��
*��������	����
*��������ֵ	��u8
*��������	��
***************************************/
u8 key_scan(void){
    //�ж�key1��key2��״̬,������1���ɿ���0
    u8 key_val = 0x00;
    static u8 key_lock = 1;

    //��KEY1��KEY2״̬���,ͬһʱ��ֻ�ܼ�⵽һ���������£�������һ�ΰ���֮ǰ��ס״̬
    if(KEY1 && key_lock){
        //����
        delay_ms(15);
        if(KEY1){
            key_val = 1;
            key_lock = 0;

        }
    }else if(KEY2 && key_lock){
        //����
        delay_ms(15);
        if(KEY2){
            key_val = 2;
            key_lock = 0;
        }
    }

    //���ɿ�������½�����
    if(KEY1==0 && KEY2==0) {
        //�ɿ�����
        delay_ms(15);
        key_lock = 1;
    }

    //���ؼ�ֵ״̬
    return key_val;

}