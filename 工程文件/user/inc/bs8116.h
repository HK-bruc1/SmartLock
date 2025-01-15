#ifndef BS8116_H
#define BS8116_H


#define BS8116_IIC_SCL_H    (GPIOB->ODR |= (1 << 6))
#define	BS8116_IIC_SCL_L    (GPIOB->ODR &= ~(1 << 6))

#define BS8116_IIC_SDA_OUT_H   (GPIOB->ODR |= (1 << 7))
#define	BS8116_IIC_SDA_OUT_L   (GPIOB->ODR &= ~(1 << 7))

#define BS8116_IIC_SDA_INT     (GPIOB->IDR & (1 << 7))

#define BS8116_IIC_SCL_INT     (GPIOB->IDR & (1 << 6))


#define BS8116ADDR_W (0x50<<1)
#define BS8116ADDR_R (0x50<<1 | 0x01)

#define BS8116_IRQ (GPIOA->IDR & (1<<1))

void BS8116_init(void);
u8 BS8116_ReadKey(void);
u8 BS8116_Key_scan(void);



#endif