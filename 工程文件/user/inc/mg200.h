#ifndef MG200_H
#define MG200_H

// 定义 MG200_DETECT 引脚（PC0，通用输入）Bit_SET或者Bit_RESET
#define MG200_DETECT_READ()         GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_0)

// 定义 MG200_PWR 引脚（PC1，通用推挽输出）
//低电平是进入低功耗模式，仅触控电路工作，只有 触控检测电路（如电容式触摸感应）保持供电，持续监测是否有手指接触传感器，功耗极低
//当用户手指接触传感器时，触控电路产生 高电平信号（或中断信号）发送给外部 MCU。MCU 收到信号后，
//立即通过 PWR_ON/OFF 引脚控制 Q2 导通，恢复 3.3V 主电源供电。主电源恢复后，指纹模块启动初始化，进入指纹采集、比对等完整工作流程。
//任务完成后再次进入低功耗
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
u8 erase_all(void);




#endif