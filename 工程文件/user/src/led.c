#include "led.h"
#include "main.h"

/***************************************
*函数名			：led_init
*函数功能		：LED灯所用IO口初始化配置
*函数参数		：无
*函数返回值	：无
*函数描述		：LED3--------PB9,LED4--------PB8
***************************************/
void led_init(void){
	//外设总线时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	//端口模式配置
	//输出类型
	//上下拉
	//输出速度
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_8;//没有共同位不冲突即可
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	//输出数据寄存器配置默认关闭
	GPIO_SetBits(GPIOB, GPIO_Pin_8 | GPIO_Pin_9);
}