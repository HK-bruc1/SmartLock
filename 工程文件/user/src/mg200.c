#include "main.h"

/**
 * @brief ָ��ģ��ĳ�ʼ��
 * 
 */
void mg200_init(void){
    //��Ӧ���ڵĳ�ʼ��
    mg200_usart6Init(115200);
    /*MG200_IO*/
    //1. �������ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
    //PC0 ͨ������
        GPIO_InitTypeDef  GPIO_InitStruct = {0};
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
        GPIO_Init(GPIOC, &GPIO_InitStruct);

    //PC1 ͨ���������
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
        GPIO_Init(GPIOC, &GPIO_InitStruct);
        //Ĭ�Ϲرյ�Դ
        GPIO_ResetBits(GPIOC, GPIO_Pin_1);
    //��ʡ�磬��ʹ�ø�Ӧ�ϵ緽ʽ������֮������
    MG200_PWR_RESET();
    //��Ӧ�ϵ緽ʽ�᲻�Ͻ����ж�(�����жϺͿ����ж�)
    //MG200_PWR_SET();//�ϵ����Ҳ�ᷢһЩ���ݵ��½����ж�
    //printf("ָ��ģ���ʼ�����\r\n");
}

/**
 * @brief mg200����ָ���,mgҪ������ݴ����ʽ
 * Ϊ�˱�����ѯ����������д���ж���
 */
void mg200_send_command(u8 command,u8 parameter){
    //����7���ֽ����ݸ�mg200
    u8 data[7] = {MG200_START_CODE, MG200_SEND_ADDR, MG200_RECV_ADDR, 0x00, 0x00, 0x00, 0x00};
    //ָ����
    data[3] = command;
    //����ָ����Ĳ�ͬ�����ܵĲ���
    data[4] = parameter;
    //У���
    u8 temp = (0x63 + 0x62 + command + parameter) & 0xff;
    data[6] = temp;
    USART6_SendBytes(data,7);
    //printf("7���ֽ����ݷ��ͳɹ�\r\n");
    //��ֹ�ϵ�ᷢ�����ݸ�MCU���⵼�±�־λ��ǰ��1������bug��
	mg200_rec_flag = 0;//ʹ����Ը�Ӧ���ݽ������
}


/***********************************************
*������    ��mg200_read_cmd
*��������  �������ж��н��յ������ݰ�
*��������  ��u8 cmd,u8 *parameter,u8 *result
*��������ֵ��u8   ����ֵ��־ͨ���Ƿ�������
*��������  ��cmd ֮ǰ���͵�ָ��  Ҫ���յ���Ӧ�����Ӧ
             *parameter    ���յ����������
             *result       ���յ�����ִ�е�Ч��
˵��һ��ʼ�ĵ��ǻ��ǶԵģ����ݰ�û�н�����ɣ��ж��߼��Ѿ�ִ�����ˡ�����
***********************************************/
u8 mg200_read_cmd(u8 cmd,u8 *parameter,u8 *result)
{

	/*�ȴ�ָ�����ݰ����ж��н������*/
	while(!mg200_rec_flag);
	
	mg200_rec_flag = 0;               //��־λ����
	/*��֤���յ������ݰ�*/
	if(mg200_buff[0] != 0x6c)   return 1;
	if(mg200_buff[1] != 0x62)   return 2;
	if(mg200_buff[2] != 0x63)   return 3;
	if(mg200_buff[3] != cmd)    return 4;
	
	*result = mg200_buff[4];               //��ȡ��ִ��Ч������
	*parameter = mg200_buff[5];            //��ȡ�����صĲ���
	//if(mg200_rec_buff[6] != 0x00)   return 5;//Ԥ��λ��ʱ����õ�����һ����0x00  ��ȡ�汾��
	
	u8 check_sum = (0x62+0x63+cmd+*result+*parameter + mg200_buff[6]) & 0xff;
	if(mg200_buff[7] != check_sum) return 6;
	
	return 0;
}


/**
 * @brief ץȡָ��ͼ�����ȡ�����㺯��(��ȡָ��ģ���ϵ���ָ��Ϣ)
 * ��ָ�����û�ע���ָ�Ʊȶ�ʱ��ʹ�ã�ָ�Ʊȶ�ʱ��ִ�и�ָ�� 1 �Σ���Ӧ�Ĳ���Ϊ 00h��
 * ��������ˡ�����
 * �����ǵڼ��βɼ�������0 �ɼ���ʾ�ɹ�
 */
u8 mg200_ReadFingerprint(u8 parameter){
    u8 parame,result;
	u8 err_val;
	//���Ͳɼ�ָ��
	mg200_send_command(0X51,parameter);
	
	//�������յ������ݰ���������һ���ȴ��������8���ֽڹ�������Ȼֻ��Ӧ�����ֽ����ݾ�ִ���ж��߼���
    //8���ֽ����ݣ�Ӧ�ø��ܲ����ӳ�
	err_val = mg200_read_cmd(0x51,&parame,&result);
		
	if(err_val == 0)   //ͨ�ųɹ���������Ӧ�ɹ����յ�8���ֽ����ݰ�
	{
		switch(result)
		{
			case 0x00: 	printf("ץȡָ��ͼ����������ȡ�ɹ�\r\n");break;
			case 0xb1:  printf("ָ�ƹ�С(small finger)\r\n");break;
			case 0xb2:  printf("��ָ�� (no finger)\r\n");break;
			case 0xb3: 	printf("ָ��λ�ù��ڿ�����Ҫ����ָ���ұ�����(left finger)\r\n");break;
			case 0xb4:  printf("ָ��λ�ù��ڿ��ң���Ҫ����ָ���������(right finger)\r\n");break;
			case 0xb5:  printf("ָ��λ�ù��ڿ��ϣ���Ҫ����ָ���±�����(up finger)\r\n");break;
			case 0xb6: 	printf("ָ��λ�ù��ڿ��£���Ҫ����ָ���ϱ�����(down finger)\r\n");break;
			case 0xb7:  printf("ʪ��ָ ��ָ�Ʊ����������ˮ�չ��ࣩ (wet finger)\r\n");break;
			case 0xb8:  printf("������ָ(dry finger)\r\n");break;
			case 0xc0: 	printf("������ȡ����\r\n");break;
			case 0xc2: 	printf("������ȡ���� (ͼ��������)\r\n");break;
			default :   printf("ץȡָ��ͼ����������ȡ����\r\n");break;
		}

	}
	else
	{
		printf("ͨ��ʧ�ܣ�\r\n");
		
		return 0xff;
	}
		
	return result;
}



/**
 * @brief ָ��ģ���ע�ắ��
 * mg200_data_flag��Ϊ�˱���CPU̫���ģ���Ӧ��û�з��أ�������Ҫһ����־λ���ж��Ƿ��Ѿ��յ����ݰ�
 * �о�û�б�Ҫ����Ϊһֱ����ָ���Ҳ��֪��Ӧ������һ�εģ�������һ�η��سɹ����ɣ����ڵ�һ�βɼ��Ķ�����
 * ���統ĳһ���ɹ�Ӧ������д������ʱ����Ϊǰ��while�жϲ��ɹ���һֱ��ָ����жϵ���һ��ʱ����ֱ������ѭ����
 * ���Ӧ��Ҫ��ȷ��һ����ֻҪ���سɹ��������ɹ��ɼ�һ�Ρ�
 * ������ѯ�£�����ȴ�������ʹ�ñ�־λ����
 * /***********************************************
*������    ��Enroll
*��������  ��ע�����û���ָ��
*��������  ��u8 ID
*��������ֵ��u8 
*��������  ��
            ����0     ��ʾע��ɹ�
            ����0xfc  ��ʾ�������쳣 ע��ʧ��
            ����0xff  ��ʾע��ʱ��ͨ��ʧ�ܵ���ע��ʧ��
            ����0xfb  �û����뵽ָ��ע����浫��û�а���ָ�ƻ���û�вɼ���������
***********************************************/
u8 mg200_register(u8 ID){
    //����ָ��������صĽ��
    u8 ret;
    //������־λ����
    static u8 touch_statu = 0;

    /*���ڲ���ʾ��ʼע�ᣬ��Ӧ�ϵ緽ʽ*/
	if(touch_statu == 0)
	{
		printf("�뽫��ָ�ŵ�ָ��ģ�鿪ʼע��ָ��\r\n");
		touch_statu = 1;  	
		printf("��һ��ָ�Ʋɼ�\r\n");
	}


    /*�ɼ�ָ��*/
        //��һ�βɼ�
        if(MG200_DETECT_READ() && touch_statu==1)
        {
            //��Ӧ�ϵ�
            MG200_PWR_SET();
            //�ȴ���Ӧ�ϵ����
            tim5Delay_Ms(30);
            //��ʱ�������
            tim9_count[3] = 0;
            //ָ�Ʊ��
            fingerprint_yellow();

            do
            {
                //��һ�βɼ��Ĳ���Ϊ0x00
                ret = mg200_ReadFingerprint(0x00);

                //��ʱ���
                if(tim9_count[3]>=5000)
                {
                    printf("�������쳣���ɼ�ʧ�ܣ�������ע��\r\n");
                    touch_statu = 0;     //���½���ע�Ṧ�ܱ�־λ
                    MG200_PWR_RESET();
                    return 0xfc;
                }
            }while(ret);//����0x00Ϊ�ɹ�֮�⣬�������㷵�ض�Ϊ�ɼ�ʧ�ܣ�retΪ0���������ѭ��
            //������־λ��ס��������ѯ��ֻҪ�ɹ��ɼ�һ��������һ�μ���ֹͣ
            touch_statu = 0xff;
            printf("��̧����ָ���ٴβɼ���һ��ָ��\r\n");
            //��������
            voice(DELETE_ASSIGNFIGNER);
        }
        //���û�̧����ҵ�һ�βɼ��ɹ����ٴδ���ʱΪ�ڶ��βɼ�,
        if(MG200_DETECT_READ()!=1 && touch_statu==0xff)
        {
            //ָ�ƻָ�
            fingerprint_lgray();
            //�ڶ��βɼ��ı�־
            touch_statu = 2;
            printf("�ڶ���ָ�Ʋɼ�\r\n");
        }

        //�ڶ��βɼ�
        if (MG200_DETECT_READ() && touch_statu==2)
        {
            tim9_count[3]=0;                //��ʱ���ʱ������
            //ָ�Ʊ��
            fingerprint_yellow();
            do
            {
                ret = mg200_ReadFingerprint(0x01);
                //��ʱ���
                if(tim9_count[3]>=5000)
                {
                    printf("�������쳣���ɼ�ʧ�ܣ�������ע��\r\n");
                    touch_statu = 0;     //���½���ע�Ṧ�ܱ�־λ
                    MG200_PWR_RESET();
                    return 0xfc;
                }
            }while(ret);	
            //ֻҪ�ɹ�һ�ξͿ�����ס�ˣ�̧���ٽ����������βɼ�
            touch_statu = 0xfe;
            printf("��̧����ָ���ٴβɼ���һ��ָ��\r\n");
            //��������
            voice(DELETE_ASSIGNFIGNER);
        }
        if(MG200_DETECT_READ()!=1 && touch_statu==0xfe)
        {
            //ָ�ƻָ�
            fingerprint_lgray();
            touch_statu = 3;
            printf("������ָ�Ʋɼ�\r\n");
        }


        //�����βɼ�
        if (MG200_DETECT_READ() && touch_statu==3)
        {
            tim9_count[3]=0;                //��ʱ���ʱ������
            //ָ�Ʊ��
            fingerprint_yellow();
            do
            {
                ret = mg200_ReadFingerprint(0x02);
                //��ʱ���
                if(tim9_count[3]>=5000)
                {
                    printf("�������쳣���ɼ�ʧ�ܣ�������ע��\r\n");
                    touch_statu = 0;     //���½���ע�Ṧ�ܱ�־λ
                    MG200_PWR_RESET();
                    return 0xfc;
                }
            }while(ret);	
            //���һ��ֻҪ�ɹ�ֱ��ע�᲻��̧��
            touch_statu = 4;
            //��������
            voice(DELETE_ASSIGNFIGNER);
            printf("�ɼ�ָ�Ƴɹ�������ע��\r\n");
        }


    /*ע��*/
        u8 para,resu;//������Ӧ�Ĳ����ͽ��
        if(touch_statu==4)
        {   
            //�ѱ�־λ�ָ�Ϊ��ʼ״̬�Ա���һ��ע��ʹ��
            touch_statu = 0;

            //ָ�Ʋ��ûָ���Ϊ�л�����ʱ�����´�ӡ
            //ע��ָ��,��������Ͳ��÷�װ�ˣ�ֱ�ӷ���ע��ָ��
            mg200_send_command(0x7f,ID);//ָ��IDע��
            //������Ӧ�����ݰ�
            if(mg200_read_cmd(0x7f,&para,&resu) != 0)   //ͨ��ʧ�ܣ����ط���ֵ������Ӧ�����д���
            {
                printf("ͨ��ʧ�ܵ���ע��ʧ�ܣ���������ע�ᣬ���ٴν���ע�Ṧ�ܣ�\r\n");
                //�ϵ�����βɼ���ֻҪ�д�����Ҫ�����ϵ��ص�һ�βɼ���ʼ
                MG200_PWR_RESET();
                return 0xff;
            }
            else
            {
                //�ж�����������
                switch(resu)
                {
                    case 0x00: printf("ע��ɹ�\r\n");break;
                    case 0x83: printf("ID ����(ID < 0 ���� ID > ����û���)����ͨ�Ŵ���\r\n");break;
                    case 0x91: printf("ע��ʧ��(�û���������)\r\n");break;
                    case 0x93: printf("�Ѿ�ע��� ID\r\n");break;
                    case 0x94: printf("ָ����ȡ���� < 3\r\n");break;
                }
            }
            //���۲����Ƿ�ɹ������ص�
            MG200_PWR_RESET() ;
            
        }   return resu;
        
    //�û����뵽ָ��ע����浫��û�а���ָ�ƻ���û�вɼ���������
    return 0xfb;
}






