#include "main.h"


/**
 * @brief ����1�ĳ�ʼ��
 * 
 * @param baud ����Ĳ�����
 */
void usart1Init(u32 baud){
    /*IO�ڿ���������*/
        //�˿�ʱ��ʹ��
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
        //������PA9�Ѿ����ӵ���USART1��TX���ţ�PA10���ӵ���USART1��RX����
        //ֻ��Ҫ��PA9��PA10����Ϊ���ù��ܣ�Alternate Function, AF������������ӳ�䵽USART1��TX��RX���ܼ��ɡ�
        //USART������Զ��������ݵķ��ͺͽ��գ��������ֶ�������Щ���š�
        GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
        GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);//������ǰ������͵������

        //�˿�ģʽ����-------����  PA9������  PA10������
        //�������
        //����ٶ�,����
        //������
        //���ù��ܼĴ�������,��0111
        GPIO_InitTypeDef GPIO_InitStruct = {0};
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
        GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
        //GPIO�ڵ��������ٶ���ֵ50MHz��������Ϊ115200û�г���GPIO�ڵ�������ٶ�
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
        GPIO_Init(GPIOA, &GPIO_InitStruct);
    

    /*ĳ�����ڿ���������*/
        //���ڿ�����ʱ��ʹ��
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	
        USART_InitTypeDef USART_InitStruct = {0};
        USART_InitStruct.USART_BaudRate = baud;
        USART_InitStruct.USART_Parity = USART_Parity_No;
        USART_InitStruct.USART_StopBits = USART_StopBits_1;
        USART_InitStruct.USART_WordLength = USART_WordLength_8b;
        //�ܲ���дһ��Ҫ��д��Ĵ�����ֵ�᲻��ӦΪ"|"���������¼Ĵ�����ֵ�����仯
        USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
        USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        // ��ʼ��USART1(Ĭ��16��������8����������ʹ�ܣ�USART_BaudRate���Զ�����)
        USART_Init(USART1, &USART_InitStruct);


    /*NVIC����*/
        //���ȼ����齨��д����������
        //��λ��ռ����λ��Ӧ��ʾ���ȼ�
        //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

        NVIC_InitTypeDef NVIC_InitStruct = {0};
        NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;//ֻ���ں��ļ�������,�޷���ת
        NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1; // ��ռ���ȼ�Ϊ1
        NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1; // ��Ӧ���ȼ�Ϊ1
        //����ʹ��USART1���ж��ź�ͨ��
        NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStruct);

    //ʹ�ܷ�һ��
    //���������ڿ��������ж�ͨ��
    //���ڽ����ж��ź�
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    //���ڿ����ж��ź�,�����ж��ַ����Ƿ������ɣ�����Ҫ��Ϊ�����������
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
    // ʹ��USART1
    USART_Cmd(USART1, ENABLE);

}


/**
 * @brief  ����һ���ֽ�����
 * 
 * @param ch һ���ֽ�����
 */
void USART1_SendChar(uint8_t ch) {
    // �ȴ��������ݼĴ���Ϊ�գ�TXE��־λΪ1��,�⺯��û�еȴ�������дһ�����ͺ���
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    // ��������
    USART_SendData(USART1, ch);
}


/**
 * @brief  �����ַ��� 
 * 
 * @param str �ַ�������ʼ��ַ
 */
void USART1_SendString(char *str) {
    //"\0",ȡֵ�õ���ʵ�Ƚϵ����ַ���ASCII��ֵ
    while (*str != '\0') {
        USART1_SendChar(*str); // ��������ַ�
        str++;
    }
}



/*
 * @brief  mg200ָ��ģ�鴮��6�ĳ�ʼ��
 *MG200-TXD ----------------------  USART6_RXD---------------PC7      ����
 *MG200-RXD----------------------  USART6_TXD--------------- PC6      ����
 *MG200_DETECT-----------------------------------------------PC0      ͨ������
 *MG200_PWR -------------------------------------------------PC1      ͨ���������
*/
void mg200_usart6Init(u32 baud){
    /*IO�ڿ���������*/
        //�˿�ʱ��ʹ��
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

        //�������ö�����ǰ������͵������
        //Ӳ�����Ѿ����ӣ�ֻ��Ҫ���ø��ù��ܣ�ӳ�䵽Ƭ������Ľӿ���
        GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6);
        GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6);

        //�˿�ģʽ����-------����  PC6������  PC7������
        GPIO_InitTypeDef GPIO_InitStruct = {0};
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
        GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
        //GPIO�ڵ��������ٶ���ֵ50MHz��������Ϊ115200û�г���GPIO�ڵ�������ٶ�
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

        GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
        GPIO_Init(GPIOC, &GPIO_InitStruct);
        //������ֱͬ��ʹ��
        GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
        GPIO_Init(GPIOC, &GPIO_InitStruct);
    

    /*ĳ�����ڿ���������*/
        //���ڿ�����ʱ��ʹ��
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);	
        USART_InitTypeDef USART_InitStruct = {0};
        USART_InitStruct.USART_BaudRate = baud;
        //��������żУ��λ
        USART_InitStruct.USART_Parity = USART_Parity_No;
        //һ��ֹͣλ
        USART_InitStruct.USART_StopBits = USART_StopBits_1;
        //ÿ������֡�а���������λ��
        USART_InitStruct.USART_WordLength = USART_WordLength_8b;
        //�ܲ���дһ��Ҫ��д��Ĵ�����ֵ�᲻��ӦΪ"|"���������¼Ĵ�����ֵ�����仯
        //���ý���ģʽ��Rx���ͷ���ģʽ��Tx��
        USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
        //������Ӳ�������ơ�ͨ��˫����ͨ�������ߣ�TX �� RX���������ݴ��䡣
        USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        //��ʼ��USART6(Ĭ��16��������8����������ʹ�ܣ�USART_BaudRate���Զ����㣬ֻҪ��Ŀ�겨���ʼ���)
        USART_Init(USART6, &USART_InitStruct);


    /*NVIC���ã����������ж�ʵ�ֱ�����ѯ*/
        //���ȼ����齨��д����������
        //��λ��ռ����λ��Ӧ��ʾ���ȼ�
        //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

        NVIC_InitTypeDef NVIC_InitStruct = {0};
        NVIC_InitStruct.NVIC_IRQChannel = USART6_IRQn;//ֻ���ں��ļ�������,�޷���ת
        NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1; // ��ռ���ȼ�Ϊ1
        NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1; // ��Ӧ���ȼ�Ϊ1
        //����ʹ��USART6���ж��ź�ͨ��
        NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStruct);

    //ʹ�ܷ�һ��
    //���������ڿ��������ж�ͨ��
    //���ڽ����ж��źţ����ռĴ����ǿգ��������ݵ��
    USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART6,USART_IT_IDLE,ENABLE);//ʹ�ܿ����ж�,�ж϶��ֽ������Ƿ��꣨��ʦ�Ĵ��룩
    //ʹ��USART6
    USART_Cmd(USART6, ENABLE);
}


/**
 * @brief  ����һ���ֽ�����
 * 
 * @param ch һ���ֽ�����
 */
void USART6_SendChar(uint8_t ch) {
    // �ȴ��������ݼĴ���Ϊ�գ�TXE��־λΪ1��,�⺯��û�еȴ�������дһ�����ͺ���
    while (USART_GetFlagStatus(USART6, USART_FLAG_TXE) == RESET);
    // ��������
    USART_SendData(USART6, ch);
}


/**
 * @brief ͨ��USART6�����ֽ�����
 * 
 * ������ּ��ͨ��USART6�첽����ͨ�Žӿڷ���һ���ֽ����ݡ������η������������е�ÿ���ֽڣ�
 * ֱ���������ݷ�����ϡ��˺���������USART6_SendChar����������ÿ���������ַ���
 * 
 * @param data ָ����������ݵ�ָ��
 * @param length ���������ݵĳ���
 */
void USART6_SendBytes(u8 *data, uint8_t length) {
    // �����������飬����ÿ���ֽ�
    for (uint8_t i = 0; i < length; i++) {
        USART6_SendChar(data[i]);
    }

}




