#ifndef _AT24CXX_H
#define _AT24CXX_H

#include "main.h"

//������ַ�궨��
#define AT24C0x_ADDR_W  0xa0    
#define AT24C0x_ADDR_R  0xa1    



#define AT24C0X_NO_ERR 0   //Ӧ��  �޴���
#define	AT24C0X_ERR1   1   //��Ӧ��
#define	AT24C0X_ERR2   2   //��Ӧ��
#define	AT24C0X_ERR3   3   //��Ӧ��
#define	AT24C0X_ERR4   4


//AT_flag��
#define AT24C02  1
#define AT24C04  2
#define AT24C08  2
#define AT24C016 2


//IICʱ���ߺ������ߺ궨��
#define IIC_SCL_H    (GPIOA->ODR |= (1 << 8))
#define	IIC_SCL_L    (GPIOA->ODR &= ~(1 << 8))

#define IIC_SDA_OUT_H   (GPIOC->ODR |= (1 << 9))
#define	IIC_SDA_OUT_L   (GPIOC->ODR &= ~(1 << 9))

#define IIC_SDA_INT     (GPIOC->IDR & (1 << 9))




void iic_IO_init(void);
void iic_star(void);
void iic_stop(void);
void iic_send_ack(u8 ack);
u8 iic_get_ack(void);
void iic_send_byte(u8 data);
u8 iic_read_byte(void);

void at24c0x_init(void);
u8 at24c0x_write_byte(u16 inner_addr,u8 data);
u8 at24c0x_read_byte(u16 inner_addr,u8 *data);
u8 AT24C0x_read_bytes(u16 inner_addr,u16 num,u8 *str);
u8 AT24C0x_write_page(u16 inner_addr,u8 num,u8 *str);
u8 AT24C0x_write_bytes(u16 inner_addr,u16 num,u8 *str,u8 AT_flag);
void AT24C0XX_Test(void);
#endif



