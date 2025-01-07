#ifndef LED_H
#define LED_H

#define LED3_ON (GPIO_ResetBits(GPIOB,GPIO_Pin_9))
#define LED3_OFF (GPIO_SetBits(GPIOB,GPIO_Pin_9))
#define LED3_TURN (GPIO_ToggleBits(GPIOB,GPIO_Pin_9))

#define LED4_ON (GPIO_ResetBits(GPIOB,GPIO_Pin_8))
#define LED4_OFF (GPIO_SetBits(GPIOB,GPIO_Pin_8))
#define LED4_TURN (GPIO_ToggleBits(GPIOB,GPIO_Pin_8))


void led_init(void);

#endif