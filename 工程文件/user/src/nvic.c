#include "main.h"



/**
 * @brief ����1�жϷ�����
 * 
 */
void USART1_IRQHandler(void){
    u8 data;
    //�궨�岻�ܸĶ����������⸳ֵ������ʼ��ַ
    static u32 zk_addr_val = ZK_ADDR;
    //���ģ���ݴ��䣬�в��������ÿ����ж��ж��Ƿ��������������
    static u32 zk_cont = 0x00141F58;


    //�ж��ǽ����ж��źŴ���
    if(USART_GetITStatus(USART1, USART_IT_RXNE)){
        //����жϱ�־λ
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
        //�����¼�(ֻ���ܲ�������),ÿ��ֻ�ܽ���һ���ֽ����ݣ��ַ����Ļ�������һ���ַ�����

        //�ж��Ƿ����ֿ���±�־,�����ճ�����
        if(zk_flag == 1) {
             //������λ�����ݹ������ֿ�����
            data = USART_ReceiveData(USART1);
            //���յ�һ���ֽڣ�ֱ��ʹ��SPIдw25q64һ���ֽ�
            w25q64_page_write(zk_addr_val,1,&data);
            //�����ƴ�
            LED4_ON;
            //��ַ����
            zk_addr_val++;
            zk_cont--;
            if(zk_cont == 0) {
                printf("�ֿ���¼���,1s���Զ���λ�豸\r\n");
                LED4_OFF;
                //�ر��ֿ���±�־
                zk_flag = 0;
                
                //�������Ź���λ��1s��λ������ι����һ�㡣��λ����10��ιһ�Ρ�
                //��Ҫ��������ֵ�� IWDG_SR �е� RVU λ����Ϊ 0��
                //RVU�� ���Ź�����������ֵ���� (Watchdog counter reload value update)
                //��ͨ��Ӳ������λ�� 1 ��ָʾ����ֵ���ڸ��¡����� VDD ��ѹ�����������ֵ���²�����
                //����Ҫ��� 5 �� RC 40 kHz ���ڣ����뼶����ͨ��Ӳ������λ��λ��
                //����ֵֻ���� RVU λΪ 0 ʱ�ſɸ��¡�ֱ���޸�����ֵ���ܻ�ʧ�ܣ���Ϊû��ȷ����һ�θ����Ƿ����
                //IWDG_PR �� IWDG_RLR �Ĵ�������д���ʱ�������Ҫ�޸ļĴ������������ȶ� IWDG_KR
                //�Ĵ���д����� 0x5555����д������ֵ����ƻ������У��Ӷ�ʹ�Ĵ������ʱ����ٴ���Ч��
                //����ζ����װ�ز�������д�� 0xAAAA��Ҳ������д�������ܡ�   
                //���д����
                IWDG->KR = 0x5555;
                while(IWDG->SR & (1U<<1));    // �ȴ�֮ǰ������ֵ������ɣ�����������ֵ�����ǻ��ߴ���
                IWDG->RLR = 250;
                IWDG->KR = 0xAAAA;
                //ֱ�������λ���Ǹ���,�ں˼�����
	            //NVIC_SystemReset();
                while(1);
            }
        }else {
            //�������,д��ȫ�������У������س���ʹ��
            //�������ݼĴ���������������֮��ʹ����жϣ�ע�����ﲻ��������λ�Ĵ���
            usart1.usart_buff[usart1.len++] = USART_ReceiveData(USART1);
        }
    }


    //�ж��ǿ����ж��źŴ���
    if(USART_GetITStatus(USART1, USART_IT_IDLE)){
        //����жϱ�־λ
        //USART_ClearITPendingBit(USART1, USART_IT_IDLE);
        //ֻ���ֶ�������⺯��û��д
        USART1->SR;
        USART1->DR;
        //�����¼�
        usart1.usart_buff[usart1.len] = '\0';//���ⲿ����������ţ���Ϊһ���ַ���
        //������ɺ󣬳������㣨������һ�ν��գ������Ѿ��浽�������ˣ�û�ж�ʧ������־λ��1��ֱ�����������д���
        usart1.len = 0;
        //�ⲿͨ����־λ�ж��Ƿ��������ݵ���
        usart1.usart_flag = 1;
    }
}

/**
 * @brief ��ʱ��9���жϷ�����
 * 
 */
TIM1_BRK_TIM9_IRQHandler(void){
    //led3�����Ʊ�־λ,��Ӱ�쿪�غ����ƣ������л������Ƶķ���
    static u8 led3_flag = 1;
    
    //�ж��ж��ź�Դ
    if (TIM_GetITStatus(TIM9, TIM_IT_Update) == SET){
        //����жϱ�־λ
        TIM_ClearITPendingBit(TIM9, TIM_IT_Update);


        //�����¼�
        //��ʱʹ��
        tim9_count[0]--;
        //led3������
        tim9_count[1]++;
        //��ȡRTCʱ���ʱ��
        tim9_count[2]++;
        //��¼ָ�Ʋɼ�ʱ�����ڳ�ʱ���
        tim9_count[3]++;


        //led3������
        if(tim9_count[1]>=2){
            tim9_count[1] = 0;
            if(led3_flag == 1){
                TIM4->CCR4++;
                if(TIM4->CCR4>=1000){
                    led3_flag = 0;
                }
            }else if(led3_flag == 0){
                TIM4->CCR4--;
                if(TIM4->CCR4<1){
                    led3_flag = 1;
                }
            }
        }



    }
}



/***********************************************
*������    ��RTC_Alarm_IRQHandler
*��������  ��rtc�����жϷ�����
*��������  ����
*��������ֵ����
*��������  ��
***********************************************/
void RTC_Alarm_IRQHandler(void)
{
	//�ж����Ӵ����ж��ź�
	if(RTC_GetITStatus(RTC_IT_ALRA))
	{
		//�����־λ
		RTC_ClearITPendingBit(RTC_IT_ALRA);
		//�����¼�
		
	}
	EXTI_ClearITPendingBit(EXTI_Line17);
	
	
}


/***********************************************
*������    ��RTC_WKUP_IRQHandler
*��������  ��rtc�����жϷ�����
*��������  ����
*��������ֵ����
*��������  ��
***********************************************/
void RTC_WKUP_IRQHandler(void)
{
	//�жϻ��Ѵ����ж��ź�
	if(RTC_GetITStatus(RTC_IT_WUT))
	{
		//�����־λ
		RTC_ClearITPendingBit(RTC_IT_WUT);
		//�����¼�
		
	}
}


/***********************************************
*������    ��USART6_IRQHandler
*��������  ����������ָ��ģ�������
*��������  ����
*��������ֵ����
*��������  ������ָ��ģ�鴫�ݹ�����8���ֽ�
***********************************************/
void USART6_IRQHandler(void){
    static u8 i = 0;

    //�ж��ǽ�������ж��źŴ���
    if(USART_GetITStatus(USART6, USART_IT_RXNE)){
        //����жϱ�־λ
        USART_ClearITPendingBit(USART6, USART_IT_RXNE);
        //�����¼�(ֻ���ܲ�������),ÿ��ֻ�ܽ���һ���ֽ�����,���ݰ��������˲��ٴν��գ����򲻽���
        mg200_buff[i++] = USART_ReceiveData(USART6);
    }

    //���������ݽ������ˣ��ж��ǿ����ж�
    if(USART_GetITStatus(USART6,USART_IT_IDLE))
    {
        //���жϱ�־λ
        USART6->SR;
        USART6->DR;
        //�����¼�
        i = 0;       //��һ�ν��մ�0�ſ�ʼ��
        //�ϵ����ᷢ���ݰ����ڷ��������mg200ʱ��mg200_rec_flag��Ϊ0
        mg200_rec_flag = 1;     //����ָ�����ɣ����Խ���ָ���
    }
}




