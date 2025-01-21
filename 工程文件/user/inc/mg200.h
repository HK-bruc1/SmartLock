#ifndef MG200_H
#define MG200_H

// ���� MG200_DETECT ���ţ�PC0��ͨ�����룩Bit_SET����Bit_RESET
#define MG200_DETECT_READ()         GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_0)

// ���� MG200_PWR ���ţ�PC1��ͨ�����������
//�͵�ƽ�ǽ���͹���ģʽ�������ص�·������ֻ�� ���ؼ���·�������ʽ������Ӧ�����ֹ��磬��������Ƿ�����ָ�Ӵ������������ļ���
//���û���ָ�Ӵ�������ʱ�����ص�·���� �ߵ�ƽ�źţ����ж��źţ����͸��ⲿ MCU��MCU �յ��źź�
//����ͨ�� PWR_ON/OFF ���ſ��� Q2 ��ͨ���ָ� 3.3V ����Դ���硣����Դ�ָ���ָ��ģ��������ʼ��������ָ�Ʋɼ����ȶԵ������������̡�
//������ɺ��ٴν���͹���
#define MG200_PWR_SET()             GPIO_SetBits(GPIOC, GPIO_Pin_1)   // ���øߵ�ƽ
#define MG200_PWR_RESET()           GPIO_ResetBits(GPIOC, GPIO_Pin_1) // ���õ͵�ƽ

//���ݰ��Ĺ̶�ֵ
#define MG200_START_CODE    0x6C
#define MG200_SEND_ADDR     0x63
#define MG200_RECV_ADDR     0x62


//ָ����ɫ
#define fingerprint_yellow()   LCD_dis_number_pic(96,120,"Z",YELLOW,1,gImage_systemPic)
#define fingerprint_lgray()   LCD_dis_number_pic(96,120,"Z",LGRAY,1,gImage_systemPic)


void mg200_init(void);
void mg200_send_command(u8 command,u8 parameter);
u8 mg200_read_cmd(u8 cmd,u8 *parameter,u8 *result);
u8 mg200_ReadFingerprint(u8 parameter);
u8 mg200_register(u8 ID);
u8 erase_all(void);




#endif