#ifndef MG200_H
#define MG200_H

// 定义 MG200_DETECT 引脚（PC0，通用输入）Bit_SET或者Bit_RESET
#define MG200_DETECT_READ()         GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_0)

// 定义 MG200_PWR 引脚（PC1，通用推挽输出）
#define MG200_PWR_SET()             GPIO_SetBits(GPIOC, GPIO_Pin_1)   // 设置高电平
#define MG200_PWR_RESET()           GPIO_ResetBits(GPIOC, GPIO_Pin_1) // 设置低电平

//数据包的固定值
#define MG200_START_CODE    0x6C
#define MG200_SEND_ADDR     0x63
#define MG200_RECV_ADDR     0x62


//指纹颜色
#define fingerprint_yellow()   LCD_dis_number_pic(96,120,"Z",YELLOW,1,gImage_systemPic)
#define fingerprint_lgray()   LCD_dis_number_pic(96,120,"Z",LGRAY,1,gImage_systemPic)


void mg200_init(void);
void mg200_send_command(u8 command,u8 parameter);
u8 mg200_read_cmd(u8 cmd,u8 *parameter,u8 *result);
u8 mg200_ReadFingerprint(u8 parameter);
u8 mg200_register(u8 ID);




#endif