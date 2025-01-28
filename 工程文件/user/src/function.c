#include "main.h"

//��ģ�����ϵ��ۺ��Ժ���


//�ֿ���º���������main����ǰ�棬�����Ļ����������ʹ�ð���2���ܸ����ֿ⣬ֻ��������ʱ��ס2���ܸ����ֿ⣬��Ϊһ�����⹦��ʹ��
void zk_update(void){
    //��ȡ����ֵ
    u8 key = key_scan();
    //�жϰ���ֵ
    if(key == 2){
        printf("���ڲ����ֿ�洢����Ҫ�Ŀռ�...");
        //0x 7f  f  f  ff
        //   ��  �� ҳ  ��ַ
        w25q64_blocks(ZK_ADDR,21);
        printf("������ɣ�\n");
        //�����ֿ���±�־
        zk_flag = 1;
    }
}








/***********************************************
*������    :main_page
*��������  :�����溯��
*��������  :u8 key
*��������ֵ:��
*��������  :page_mode = 1
************************************************/
void main_page(u8 key)
{
    // ����һ����־λ����ʾ��ǰ����ɫ״̬
    static int color_flag = 0;
    //����ʱ������,��Ϊ��ε��ñ��ⷴ������
    static u8 timer_buff[20];
	//���뿪����ʽ
		//�������Ա����ı�־��ֻ�����뿪����ʽ���Խ��뵽����Ա���棬����ֻ�п��Ź��ܣ�
		u8 sta;
		//�����������뿪����,�Ž������Ա����ʹ�ü�ֵ���ʸ�����
		sta = open_passward(key);
		if(sta == 1){
			//���ں����в����ý�����һ��ҳ�棬��while�в���ͨ����־λ��ѯ������Ӧ���漴��
			page_mode = 2;
		}
	//ָ�ƿ�����ʽ���͹���ģʽ��ָ��ģ�鱻������ѯ��
	open_fingerprint();
	//��Ƭ������ʽ
	open_card();



    //���������ʱ���ʱ��,һ����ˢ��һ��,д���жϺ����У�ʱ����ʾ���ܿ���
    if(tim9_count[2]>=60000){
        tim9_count[2] = 0;
        //��ȡ���ں�ʱ��
        RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
        RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
        //��ʾʱ�䣬����ת��Ϊ�ַ�����ʽ
        sprintf((char *)timer_buff,"%02d:%02d",RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes);
        //ʹ��48*48��
        LCD_dis_number_pic(0,58,timer_buff,LGRAY,1,gImage_systemPic);
    }

}


/***********************************************
*������    :open_passward
*��������  :���뿪��
*��������  :u8 bs8116_key
*��������ֵ:u8
*��������  :����1 ���ųɹ�
           ����0 ����ʧ��
************************************************/
u8 open_passward(u8 bs8116_key){
    //���������״̬����ֵ
    u8 sta = 0xff;
    //��һ�ο�����־λ��ֵ,����ֻ���ȡһ�ε��ǻ�����Σ���Ҫ������������Ȼ�ڶ��ξͻ����
    static u8 open_val;
    //�����¼ҳ��״̬��
    static u8 pass_page_flag = 1;
    //���뿪���Ľ���Բ������
    static u8 x = 45,y = 200;
    //������������Բ���״̬��
    static u8 pass_flag = 1;
    //���ڴ洢���������ֵ
	static u8 pass_word_1[7] = {0};
	static u8 pass_word_2[7] = {0};
    //���ڼ������������Բ�ĸ���
	static u8 pass_i = 0;



    /*���������¼�Լ�������,������������ʱ��ֻ֤����һ�Σ�������Ч��������CPUЧ���½�*/
    if(pass_page_flag){
        //����һ�ο�����־λ
        at24c0x_read_byte(10,&open_val);
        //���뿪������
        LCD_dis_pic(0,0,gImage_systemPic);
        x=45;
        for(u8 i=0;i<6;i++)
		{
			LCD_Ring(x,y,8,LGRAY);
			x+=30;
		}
        
        //��ס�����棬��ֹ����ˢ��
        pass_page_flag = 0;
        //�����λ�����㻭ʵ��Բ
        x=45;


        //ÿһ�ν��������涼Ҫ������ʾʱ��
        tim9_count[2] = 60000;
		//���ǵ�һ�ο����Ŵ�ӡָ�ƣ���һ�ο���ֻ�����ó�ʼ����
		if(open_val == OPEN_FLAG){
			//��ӡָ��ͼƬ
        	LCD_dis_number_pic(96,120,"Z",LGRAY,1,gImage_systemPic);
		}

    }




    /*�ж��ǵ�һ�ο�������Ҫ��������*/
    if(open_val != OPEN_FLAG){
        //�����һ�γ�ʼ�������������
        if(pass_flag==1){
            LCD_dis(70,130,"������������",LIGHTBLUE,0,0xff,16);
            voice(INPUT_NEW_PASSWORD);
            //��ס���������������������ֹ��������
            pass_flag=0;
        }

        //ֻ����Ч����ֵ
        if(bs8116_key !='*' && bs8116_key!='#' && bs8116_key!=0xff){
            //��һ����������
            if(pass_i<6){
                //�������
                pass_word_1[pass_i] = bs8116_key;
                pass_i++;
                //��Ļ����ʵ��Բ
                LCD_shiRing(x,y,8,LGRAY);
                //Բ�������ƶ�
                x+=30;

                //pass_i��6�ͱ�ʾ��һ�������������
                if(pass_i==6){
                //��ʵ��Բ���������䱳��ͼƬ������ɫֵ
                LCD_xy_clear(45-8,200-8,166,16,0xffff,1,gImage_systemPic);
                //���»�����Բ
                x=45;
                for(u8 i=0;i<6;i++)
                {
                    LCD_Ring(x,y,8,LGRAY);
                    x+=30;
                }

                //����ʾ֮ǰ��������Ҫ�ı���������ɾ������������ڵ����򱳾�����ֵȫ���ĳɶ�ӦͼƬ��ɫֵ
                LCD_xy_clear(70,130,100,16,0xffff,1,gImage_systemPic);
                LCD_dis(70,130,"���ٴ�����������",LIGHTBLUE,0,0xff,16);
                voice(INPUT_NEW_PASSWORD_AGAIN);
                //Բ�Ĺ�λ���Ա���һ��ʹ��
                x=45;
                
                }
            }
            //�ڶ�����������
            else if(pass_i>=6){
                pass_word_2[pass_i-6] = bs8116_key;
                pass_i++;
                //��Ļʵ��Բ�ٴθ���
                LCD_shiRing(x,y,8,LGRAY);
                //Բ�������ƶ�
                x+=30;

                //��pass_i��12ʱ���ͱ�ʾ�ڶ��������������
                if(pass_i==12){
                    //�Ա����������Ƿ�һ��
                    if(strcmp((char *)pass_word_1,(char *)pass_word_2)==0){
                        //���������at24
                        AT24C0x_write_bytes(11,sizeof(pass_word_2),pass_word_2,AT24C04);
                        //д���һ�ο�����־λ
                        at24c0x_write_byte(10,OPEN_FLAG);

                        //�ص����������¼����,����ˢ��ͼƬ�Ϳ���Բ,������״̬����
                        pass_page_flag = 1;
                        //���������������¼���
                        pass_i = 0;
                        //���״ο�������������״̬��,��Ϊ���ó�ʼ��������Ͳ������һ�ο����߼���
                        //����������������߼�Ҳ��Ҫ����Ҫ���ǽ���
                        pass_flag = 1;
                        voice(SETTING_SUCCESS);
                    }
                    else{
                        //�ص����������¼����,����ˢ��ͼƬ�Ϳ���Բ,������״̬����
                        //���¿�ʼ�����������
                        pass_flag = 1;//���״ο�������������״̬������Ϊ���벻һ����Ҫ���²���
                        pass_i = 0;
                        pass_page_flag = 1;
                        voice(PASSWORD_INCONFORMITY);
                    }
                }
            } 
        }
    }  


    /*���ǵ�һ�ο�������Ҫ��������Ա�*/
    //������һ��������������󣬽��뵽������������߼�
    else{
        //�������������������(ֻ����һ�μ���),pass_flag״̬�����ǽ����һ�ο����߼�������Ҳ����ʹ��
        if(pass_flag==1){
            voice(INPUT_ADMIN_PASSWORD);
            pass_flag = 0;

        }
        //�Ȼ�ȡ�û��������λ���룬��ǰpass_i��ֵ�Ѿ�����
        if(bs8116_key !='*' && bs8116_key!='#' && bs8116_key!=0xff){
            //��һ����������
            if(pass_i<6){
                //�������
                pass_word_1[pass_i] = bs8116_key;
                pass_i++;
                //��Ļ����ʵ��Բ
                LCD_shiRing(x,y,8,LGRAY);
                //Բ�������ƶ�
                x+=30;

                //pass_i��6�ͱ�ʾ�����������
                if(pass_i==6){
                    //��ȡ����Ա�
                    AT24C0x_read_bytes(11,sizeof(pass_word_2),pass_word_2);
                    if(strcmp((char *)pass_word_1,(char *)pass_word_2)==0){
                        voice(DOOROPEN_SUCCESS);
                        door_open();
                        //����֮�󣬰�����״̬���򿪣��ص������¼����
                        pass_page_flag = 1;
                        pass_i = 0;
                        pass_flag = 1;

                        sta = 1;
                    }
                    else {
                        //����ʧ�ܣ��ص������¼����
                        pass_page_flag = 1;
                        pass_i = 0;
                        voice(DOOROPEN_FAIL);
                        pass_flag = 1;
                        
                        sta = 0;
                    }
                }
            }
        }
    }
    //ÿ��ɨ���޲����Ļ�����0xff
    return sta;
}


void open_fingerprint(void){
	//�Ƿ��ųɹ��ı�־λ
	static u8 open_door_flag = 0;
	//�������ղ�������ֵ
    u8 rec_parameter,ret = 0xfe;
	//ָ��ģ��Ĵ���״̬
	static u8 finger_touch_sta = 0;

	//������ѯָ��ģ��Ĵ���״̬���͹���ģʽ�£�
	if(MG200_DETECT_READ() && finger_touch_sta == 0){
		//��ֹһ�δ�����δ���
		finger_touch_sta = 1;

		//����������
		MG200_PWR_SET();
		tim5Delay_Ms(30);
		//�ɼ�ָ������
			//��ʱ�������
			tim9_count[3] = 0;
			//ָ�Ʊ��
        	fingerprint_yellow();
			do
			{
				//��һ�βɼ��Ĳ���Ϊ0x00
				ret = mg200_ReadFingerprint(0x00);

				//��ʱ���,�ɼ�ʧ��
				if(tim9_count[3]>=5000)
				{
					printf("�������쳣���ɼ�ʧ�ܣ�������\r\n");
					MG200_PWR_RESET();
					//ָ�ƻָ�
        			fingerprint_lgray();
					//�ɿ���ָ��������
					voice(DOOROPEN_FAIL);
					//״̬�⿪
					finger_touch_sta = 0;
					return;
				}
			}while(ret!=0x00);//����0x00Ϊ�ɹ�֮�⣬�������㷵�ض�Ϊ�ɼ�ʧ�ܣ�retΪ0���������ѭ��

		//ָ�Ʋɼ�֮��ʼ�ȶ�
        mg200_send_command(0x71,0x00);
        //�ȴ���������Ӧ�����ݰ�
        if(mg200_read_cmd(0x71,&rec_parameter,&ret))
        {
            //���㷵��ֵ�������ݰ��д���
            printf("ͨ��ʧ��,�������ݰ�����\r\n");
            MG200_PWR_RESET();
			//ָ�ƻָ�
			fingerprint_lgray();
			//�ɿ���ָ��������
			voice(DOOROPEN_FAIL);
			//״̬�⿪
			finger_touch_sta = 0;
            return;
        }

        //���ݰ�û�д��������£��жϷ��صĲ������
        switch(ret)
        {
            case 0x00:printf("ָ�ƶԱȳɹ�:%d\r\n",rec_parameter);break;
            default:printf("��ָ�Ʋ�����\r\n");break;
        }

		if(ret==0x00){
			//�ж�ָ�ƶԱȽ��������ɹ��������,ֱ�����Ž���
			door_open();
			//����״̬��־λ��1
			open_door_flag = 1;
			//��������
			voice(DOOROPEN_SUCCESS);
			//��ʼ��ʱ
			tim9_count[4] = 0;
			//5s֮���Զ�����
		}else{
			//ָ�ƶԱ�ʧ�ܣ��ɿ���ָ��������
			voice(DOOROPEN_FAIL);
			//״̬�⿪
			finger_touch_sta = 0;
		}
	}
	


	//�ɿ�ָ����ɫ�ָ�
	if(MG200_DETECT_READ()==0 && finger_touch_sta==1){
		//ָ�ƻָ�
		fingerprint_lgray();
		//һ�βɼ��Ա����
		finger_touch_sta = 0;
		//�ص�Դ
		MG200_PWR_RESET();
	}

	//�ڳɹ����ŵ������
	if(open_door_flag==1){
		//�����ж�ָ�ƿ�������Զ������߼�
		if(tim9_count[4]>=5000){
			printf("close_door");
			door_close();
			//�ѱ�־λ��0
			open_door_flag = 0;
		}
	}
}


void open_card(void){
	//�����뿨Ƭ�������Ļ�����ά�������ݲ����ȡ���������ﵥ����ȡһ��
	static u8 ad_flag = 1;
	u8 sta;
	u8 picc_i,picc_j;
	//�˿�Ƭ�Ƿ�ע��ı�־
	u8 register_flag = 0;


	//ִ��һ��
	if(ad_flag)
	{
		//��AT24�ж�����ƬID���ݵ���ά������
		AT24C0x_read_bytes(31,sizeof(picc),picc);
		//�������ҳ���ֻ��ȡһ�Σ����ųɹ���Żָ�
		ad_flag = 0;
	}




	//���ȫ���õ���ʼ��������
	sta = ReadCardData(62,picc_read_data,picc_id);
	if(sta == MI_OK){
		//�ȱ�����ά���ݣ����˿�Ƭ��û���ڱ���ע���
		for(picc_i=0;picc_i<9;picc_i++)
		{
			//���кŶԱȳɹ����ֽ�����
			u8 temp_count = 0;
			for(picc_j=0;picc_j<4;picc_j++)
			{
				//���жϴ˿�Ƭ��û���ڱ���ע���
				if(picc[picc_i][picc_j] == picc_id[picc_j])
				{
					//��Ƭ���ĸ��ֽ����ȫ����ȣ�˵���Ѿ�ע���������ѭ��
					temp_count++;
				}
			}

			//����ĸ��ֽڶ���ȣ�˵���Ѿ�ע���������ѭ��
			if(temp_count == 4)
			{
				//����ѭ��,��������,�������趼����ִ����
				register_flag = 1;
				printf("�˿�Ƭ�Ѿ�ע��! IDΪ:%d\r\n",picc_i+1);
				break;
			}
		}

		//�Ѿ��Ա��˿�Ƭ���кţ��˿�Ƭ�Ѿ�ע��������������ݿ��������ж��Ƿ��п���Ȩ��
		if(register_flag == 1){
			for(int i=0;i<16;i++){
				//�����һ���ֽ����ݲ���ȵĻ�������û��Ȩ��
				if(picc_read_data[i] != picc_data[i])
				{
					//����ѭ��,��������,�������趼����ִ����
					printf("�˿�Ƭû��Ȩ��!");
					voice(DOOROPEN_FAIL);
					//ֱ�ӽ����������½���Ѱ������
					return;
				}
			}
			//����ѭ��û���������ʹ�����Ȩ��
			door_open();
			//��������
			voice(DOOROPEN_SUCCESS);
			//���ųɹ��󣬰�״̬�ָ����Ա���һ�ν���ʱ���ȡ���µ�ע������
			ad_flag = 1;
		}else {
			//û��ע�ỹ�ǿ���ʧ��
			printf("�˿�Ƭû��ע��!");
			voice(DOOROPEN_FAIL);
		}
	}
}


/***********************************************
*������    :admin_page
*��������  :����Ա���溯��
*��������  :u8 key
*��������ֵ:��
*��������  :page_mode = 2
************************************************/
void admin_page(u8 key)
{
    //״̬��
    static u8 ad_flag = 1;

    if(ad_flag==1){
        //������λ��
		printf("\r\n����Ա����:\r\n");
		printf("��1���������\r\n");
		printf("��2��ָ�ƹ���\r\n");
		printf("��3����Ƶ������\r\n");
		printf("��4���������ȵ���\r\n");
		printf("��5���ָ���������\r\n");
		printf("��#���˳��������\r\n");

		//LCD��Ļ��ʾ
		LCD_dis_pic(0,0,gImage_systemPic);

		LCD_dis(40,48,"����Ա����",LIGHTBLUE,0,0xff,32);
		LCD_dis(5,83,"��1���������",LIGHTBLUE,0,0xff,16);
		LCD_dis(5,108,"��2��ָ�ƹ���",LIGHTBLUE,0,0xff,16);
		LCD_dis(5,133,"��3����Ƶ������",LIGHTBLUE,0,0xff,16);
		LCD_dis(5,158,"��4���������ȵ���",LIGHTBLUE,0,0xff,16);
		LCD_dis(5,183,"��5���ָ���������",LIGHTBLUE,0,0xff,16);
		LCD_dis(5,208,"��#���˳�����Ա����",LIGHTBLUE,0,0xff,16);

        //��סҳ�棬������ֻˢ��һ��ҳ��
        ad_flag = 0;
    }




    //��ҳ��Ĺ���ѡ�񣨸�����ѯ��
    switch(key){
        //��������״̬���⿪���Ա����������ʾ
        case '1':page_mode = 3;ad_flag = 1;break;//���뵽����������,״̬���⿪
        case '2':page_mode = 4;ad_flag = 1;break;//���뵽ָ�ƹ������,״̬���⿪
        case '3':page_mode = 5;ad_flag = 1;break;//���뵽��Ƶ���������,״̬���⿪
        case '4':page_mode = 6;ad_flag = 1;break;//���뵽�������ȵ��ڽ���,״̬���⿪
        case '5':page_mode = 7;ad_flag = 1;break;//���뵽�ָ��������ý���,״̬���⿪
        case '#':page_mode = 1;ad_flag = 1;break;//���ص������¼������,״̬���⿪
    }
}



/***********************************************
*������    :password_page
*��������  :���������溯��
*��������  :u8 key
*��������ֵ:��
*��������  :page_mode = 3
************************************************/
void password_page(u8 key)
{
    static u8 ad_flag = 1;
    //һ��key����һ�Σ�x,y������Ҫ��������
	static u8 x=45,y=95;
	static u8 pass_i = 0;
	static u8 pass_word_1[7] = {0};
	static u8 pass_word_2[7] = {0};
	
	u8 i;


	//ִ��һ��    ������ʾ
	if(ad_flag)
	{
        //��ֹ������;����δ��ɣ���Ҫ��գ���Ȼpass_iΪ�ϴεľ�ֵ,�±���գ������ֵ�ᱻ���ǣ����õ���
        pass_i = 0;
		//������λ����ʾ
		printf("\r\n�޸Ĺ���Ա����\r\n");
		printf("��#��������һ���˵�\r\n");
		printf("��*������������\r\n");
		
		
		//LCD��Ļ��ʾ
		LCD_dis_pic(0,0,gImage_systemPic);
		LCD_dis(9,48,"�޸Ĺ���Ա����",LIGHTBLUE,0,0xff,32);
		LCD_dis(60,90,"��һ����������",LIGHTBLUE,0,0xff,16);
        //����Բ����
		x = 45;
		for(i = 0;i < 6;i++)
		{
			LCD_Ring(x,120,6,LGRAY);
			x += 30;
		}

		LCD_dis(60,140,"�ڶ�����������",LIGHTBLUE,0,0xff,16);
		x = 45;
		for(i = 0;i < 6;i++)
		{
			LCD_Ring(x,170,6,LGRAY);
			x += 30;
		}
		x = 45;

        //��������������ʾ
		LCD_dis(5,220,"�����桾*��",LIGHTBLUE,0,0xff,16);
		LCD_dis(145,220,"��#����һ��",LIGHTBLUE,0,0xff,16);
		
		ad_flag = 0;
	}


    //�޸����빦�ܳ���
	if(key != '*' && key != '#' && key != 0xff)
	{
		
		//��һ����������
		if(pass_i<6)
		{
			pass_word_1[pass_i] = key;
			pass_i++;
			LCD_shiRing(x,120,8,LGRAY);
			x+=30;
			
			//��һ��������6�����������Ļ����ȡ�����ʾ��
			if(pass_i == 6)
			{
				x = 45;
				voice(0x0e);
			}
		}
		
		//�ڶ�����������
		else if(pass_i >= 6 )
		{
			pass_word_2[pass_i-6] = key;
			pass_i++;
			LCD_shiRing(x,170,8,LGRAY);
			x+=30;
			//�ڶ���������6������  �Ա�����
			if(pass_i == 12)
			{
				//��������һ��
				if(strcmp((char *)pass_word_1,(char *)pass_word_2) == 0)
				{
					//������洢��AT24��11�ŵ�ַ
					AT24C0x_write_bytes(11,sizeof(pass_word_1),pass_word_1,AT24C04);
					//�ص���¼����
					pass_i = 0;
					page_mode = 1;
					ad_flag = 1;
				}
				//�������벻һ��
				else
				{
					voice(0x10);
					pass_i = 0;
					ad_flag = 1;
				}
			}
			
		}	
	}


    switch(key)
	{
		case '#': page_mode = 2; ad_flag = 1;break;    //������һ���˵�
		case '*': page_mode = 1; ad_flag = 1;break;    //����������
		
	}
}



/***********************************************
*������    :mg200_page
*��������  :ָ�ƹ�����溯��
*��������  :u8 key
*��������ֵ:��
*��������  :page_mode = 4
************************************************/
void mg200_page(u8 key)
{
	static u8 ad_flag = 1; 
	//�״ν���ָ�ƹ�������־
	u8 open_flag;
	
	
	//ִ��һ��
	if(ad_flag)
	{
		//������λ����ʾ
		printf("\r\nָ�ƹ������:\r\n");
		printf("��1��ע��ָ��\r\n");
		printf("��2��ɾ��ָ��IDָ��\r\n");
		printf("��3��ɾ������ָ��\r\n");
		printf("��4��ʶ��ɾ��ָ��\r\n");
		printf("��#���ص���һ���˵�\r\n");
		printf("��*���ص�������\r\n");
		
		
		//LCD��Ļ��ʾ
		LCD_dis_pic(0,0,gImage_systemPic);
		LCD_dis(26,48,"ָ�ƹ������",LIGHTBLUE,0,0xff,32);
		LCD_dis(5,83,"��1��ע��ָ��",LIGHTBLUE,0,0xff,16);
		LCD_dis(5,108,"��2��ɾ��ָ��IDָ��",LIGHTBLUE,0,0xff,16);
		LCD_dis(5,133,"��3��ɾ������ָ��",LIGHTBLUE,0,0xff,16);
		LCD_dis(5,158,"��4��ʶ��ɾ��ָ��",LIGHTBLUE,0,0xff,16);	
		LCD_dis(5,220,"�����桾*��",LIGHTBLUE,0,0xff,16);
		LCD_dis(145,220,"��#����һ��",LIGHTBLUE,0,0xff,16);
		

		//����Ƿ�Ϊ��һ�ν���ָ�ƹ����ѳ�ʼָ��ID������д��洢оƬ�У�
		at24c0x_read_byte(20,&open_flag);
		if(open_flag != OPEN_FLAG)
		{
			//���ָ��ģ��(��һ�ν���ָ�ƹ���˵��֮ǰ��ָ�ƶ���Ҫ����)
			erase_all();
			//��ԭʼ����洢��AT24��,ʹ��0xff��Ϊ��ʼ��Чֵ
			memset(mg200_id,0xff,sizeof(mg200_id));
			AT24C0x_write_bytes(21,sizeof(mg200_id),mg200_id,AT24C04);
			//�ѵ�һ�μ���־λд�뵽AT(��������AT24C04)
			at24c0x_write_byte(20,OPEN_FLAG);
		}

		
	
		ad_flag = 0;
	}
	
	
	switch(key)
	{
		case '1':page_mode = 41;ad_flag=1;break;
        case '2':page_mode = 42;ad_flag=1;break;
        case '3':page_mode = 43;ad_flag=1;break;
        case '4':page_mode = 44;ad_flag=1;break;
		case '#':page_mode = 2;ad_flag=1;break;
		case '*':page_mode = 1;ad_flag=1;break;
	}
}


/***********************************************
*������    :Enroll_user_page
*��������  :ע���û�ָ�ƽ��溯��
*��������  :u8 key
*��������ֵ:��
*��������  :page_mode = 41
*ָ��ģ����Ա���ָ�ƣ����ⲻ�ɼ����ò����Ѿ������ָ��ID�б�ֻ��ͨ��AT24C02������ʹ��ID�б���Ϣ��ͨ�����б����ģ���б����ָ��
************************************************/
void Enroll_user_page(u8 key)
{
	static u8 ad_flag = 1;
	//ָ��ע��Ĳ���״̬
	static mg200_register_sta = 1;
	//��ʾ����,ע��ɹ�֮����Ҫ�����궨λ
	static u16 x = 30;
	u8 mg200_i;
	//Ҫ¼���ָ��ID
	static u8 mg200_ID;
	
	//ִ��һ��
	if(ad_flag)
	{
		//��������(�ڲɼ�����д��ã���ʱ����ʹ��)
		//voice(REGISTER_FINGER);
		//������λ����ʾ
		printf("\r\nע���û�ָ�ƽ���:\r\n");
		printf("��#���ص���һ���˵�\r\n");
		printf("��*���ص�������\r\n");
		//LCD��Ļ��ʾ
		//Ϊʲô��˿������Ϊÿһ������ı���ɫ��һ���ģ�ˢ��ʱ���������Ե�������Ļ���л��У���Ϊ����������Ļ����ɫ���ı���
		LCD_dis_pic(0,0,gImage_systemPic);
		LCD_dis(24,48,"ע���û�ָ��",LIGHTBLUE,0,0xff,32);
		LCD_dis(0,220,"��*��������",LIGHTBLUE,0,0xff,16);
		LCD_dis(150,220,"��һ����#��",LIGHTBLUE,0,0xff,16);
		LCD_dis_number_pic(96,120,"Z",LGRAY,1,gImage_systemPic);


		//��AT24�ж���ָ��ID���ݵ�������
		AT24C0x_read_bytes(21,sizeof(mg200_id),mg200_id);
		//������ID��ӡ����Ļ��
		x = 30;
		//�Ѵ����9��ָ��ID�ó���
		for(mg200_i=0;mg200_i<9;mg200_i++)
		{
			//ɸѡ��ʾ��Чָ��ID
			if(mg200_id[mg200_i]!= 0xff)
			{
				printf("%u  ",mg200_id[mg200_i]);
				LCD_dis_ch(x,80,mg200_id[mg200_i]+48,LIGHTBLUE,0,0xff,32);
				x+=20;
			}
		}

		//���������ҵ���0xff��Ԫ�ص��±꣬��Ϊ��һ��¼��ָ�Ƶ�����洢�±�
		for(mg200_i=0;mg200_i<9;mg200_i++)
		{
			if(mg200_id[mg200_i]== 0xff)
			{
				break;
			}
		}
		//ע��ID�����±�+1 0xff��ʾ��Чֵ��ע��ID������1~9
		mg200_ID = mg200_i+1;
		//9��Ԫ�ر����꣬��ômg200_i�����9��mg200_i+1=10
		if(mg200_ID==10)
		{
			printf("ע��ָ�������Դ�����\r\n");
		}

		ad_flag = 0;
	}

	//ID����Ҫ��Ϳ��Կ�ʼɨ��ע�ắ��
	if(mg200_ID<10)
	{
		//����0xfb  �û����뵽ָ��ע����浫��û�а���ָ�ƻ���û�вɼ���������
		mg200_register_sta = mg200_register(mg200_ID);
		//ע��ɹ�
		if(mg200_register_sta == 0x00)
		{
			//��ע���IDд�������У�ID���±����1
			mg200_id[mg200_ID-1] = mg200_ID;
			//������д��AT24
			AT24C0x_write_bytes(21,sizeof(mg200_id),mg200_id,AT24C04);
			printf("��ע��%d��ID\r\n",mg200_ID);
			//����ˢ��ҳ�棬��ʾ�µ���ЧID
			ad_flag = 1;
		}
	}



	
	
	switch(key)
	{
		case '#':page_mode = 4;ad_flag=1;break;
		case '*':page_mode = 1;ad_flag=1;break;
	}

	
}




/***********************************************
*������    :erase_user_one_page
*��������  :ɾ��ָ��IDָ�ƽ��溯��
*��������  :u8 key
*��������ֵ:��
*��������  :page_mode = 42
************************************************/
void erase_user_one_page(u8 key)
{
	static u8 ad_flag = 1;
	//ָ��ɾ���Ĳ���״̬
	static mg200_erase_sta = 1;
	//��ʾ����,ɾ����ɺ�ֱ��ˢ��ҳ���ӡ�µ���ЧID������Ҫ������
	u16 x = 30;
	u8 mg200_i;
	//Ҫɾ����ָ��ID
	u8 mg200_ID;
	
	
	//ִ��һ��
	if(ad_flag)
	{
		//��������
		voice(DELETE_ASSIGN_FINGER);
		//������λ����ʾ
		printf("\r\nɾ��ָ��ָ�ƽ���:\r\n");
		printf("��ѡ��ָ��ID\r\n");
		printf("��#���ص���һ���˵�\r\n");
		printf("��*���ص�������\r\n");

		//LCD��Ļ��ʾ
		LCD_dis_pic(0,0,gImage_systemPic);
		LCD_dis(24,48,"ɾ��ָ��ָ��",LIGHTBLUE,0,0xff,32);
		LCD_dis(0,220,"��*��������",LIGHTBLUE,0,0xff,16);
		LCD_dis(150,220,"��һ����#��",LIGHTBLUE,0,0xff,16);
		

		//��AT24�ж���ָ��ID���ݵ�������
		AT24C0x_read_bytes(21,sizeof(mg200_id),mg200_id);
		//������ID��ӡ����Ļ��
		x = 30;
		//�Ѵ����9��ָ��ID�ó���
		for(mg200_i=0;mg200_i<9;mg200_i++)
		{
			//ɸѡ��ʾ��Чָ��ID
			if(mg200_id[mg200_i]!= 0xff)
			{
				printf("%u  ",mg200_id[mg200_i]);
				LCD_dis_ch(x,80,mg200_id[mg200_i]+48,LIGHTBLUE,0,0xff,32);
				x+=20;
			}
		}
		
		ad_flag = 0;
	}

	//ɾ��ָ��IDָ�ƣ�keyΪ��Чֵ��ִ��ɾ������
	if(key != 0xff && key != '#' && key != '*'){
		//��ֵ��ID��ת��
		mg200_erase_sta = EraseOne(key-48);
		//ɾ���ɹ�����������ҲҪɾ��
		if(mg200_erase_sta == 0x00)
		{
			//ID���±����1,�ָ�Ĭ��ֵ
			mg200_id[key-48-1] = 0xff;
			//����������д��AT24
			AT24C0x_write_bytes(21,sizeof(mg200_id),mg200_id,AT24C04);
			//���Բ���
			voice(SETTING_SUCCESS);

			//�ͷ�״̬��������ˢ�µ�ǰҳ�棬��ӡ�µ���ЧID
			ad_flag = 1;
			
		}
	}

	
	switch(key)
	{
		case '#':page_mode = 4;ad_flag=1;break;
		case '*':page_mode = 1;ad_flag=1;break;
	}
}





/***********************************************
*������    :erase_user_all_page
*��������  :ɾ������ָ�ƽ��溯��
*��������  :u8 key
*��������ֵ:��
*��������  :page_mode = 43
************************************************/
void erase_user_all_page(u8 key)
{
	static u8 ad_flag = 1;

	//ִ��һ��
	if(ad_flag)
	{	
		//��������
		voice(DELETE_ALLFINGER);
		//������λ����ʾ
		printf("\r\nɾ������ָ��?:\r\n");
		printf("��*��ȷ��\r\n");
		printf("#��ȡ��\r\n");
		
		
		//LCD��Ļ��ʾ
		LCD_dis_pic(0,0,gImage_systemPic);
		LCD_dis(24,48,"ɾ������ָ��",LIGHTBLUE,0,0xff,32);
		LCD_dis(56,85,"ȷ��ɾ��",LIGHTBLUE,0,0xff,32);
		LCD_dis_number_pic(96,126,"?",RED,1,gImage_systemPic);
		LCD_dis(0,220,"ȷ��(*)",LIGHTBLUE,0,0xff,16);
		LCD_dis(180,220,"ȡ��(#)",LIGHTBLUE,0,0xff,16);
		
		ad_flag = 0;
	}

	

	
	if(key == '*')     //ȷ��ɾ�� �� �ص��ϼ��˵�
	{
		//ɾ���Ķ���
			//���ָ��ģ��
			erase_all();
			//�ѱ���ָ�����ݸ���Ϊ��Чֵ0xff
			memset(mg200_id,0xff,sizeof(mg200_id));
			AT24C0x_write_bytes(21,sizeof(mg200_id),mg200_id,AT24C04);
			printf("��ɾ������ָ��\r\n");
			//��������
			voice(SETTING_SUCCESS);
		
		//���ϼ��˵�
		page_mode = 4;
		ad_flag=1;
	}
	
	else if(key == '#')   //ȡ���ᵽ�ϼ��˵�
	{
		page_mode = 4;
		ad_flag=1;
	}

}




/***********************************************
*������    :erase_user_match_page
*��������  :ʶ��ɾ��ָ�ƽ��溯��
*��������  :u8 key
*��������ֵ:��
*��������  :page_mode = 44
************************************************/
void erase_user_match_page(u8 key)
{
	static u8 ad_flag = 1;
	//��ʾ����
	static u16 x = 30;
	static u8 mg200_i = 0;
	//Ҫɾ����ָ��ID�б���ʼ����ЧIDΪ0
	static u8 mg200_ID[9] = {0};

	
	//ִ��һ��
	if(ad_flag == 1)
	{
		//��������(��ָ�Ʋɼ�ʱ�������ã�����ʱ����һ����)
		//voice(DELETE_ASSIGNFIGNER);
		//������λ����ʾ
		printf("\r\nʶ��ɾ��ָ�ƽ���\r\n");
		printf("ʶ��ָ��\r\n");
		printf("��*��ȷ��\r\n");
		printf("��#��ȡ��\r\n");
		
		//LCD��Ļ��ʾ
		LCD_dis_pic(0,0,gImage_systemPic);//����ͼƬΪ����
		LCD_dis(25,48,(u8 *)"ʶ��ɾ��ָ��",LIGHTBLUE,0,0xff,32);
		LCD_dis_number_pic(96,120,(u8 *)"Z",LGRAY,1,gImage_systemPic);
		LCD_dis(160,220,"��#������",LIGHTBLUE,0,0xff,16);
		LCD_dis(0,220,"ȷ�ϡ�*��",LIGHTBLUE,0,0xff,16);

		//��AT24�ж���ָ��ID���ݵ�������
		AT24C0x_read_bytes(21,sizeof(mg200_id),mg200_id);
		//ÿ�ν���ˢ��һ������
		x = 30;
		//ʶ���ָ�Ƹ���ҲҪ����
		mg200_i = 0;
		//���б����³�ʼ����Чֵ���ȴ�����ʶ��
		for(int l=0;l<9;l++){
			mg200_ID[l] = 0;
		}

		ad_flag = 0;
	}


	//������ѯָ�ƶԱȺ���
	//ָ�ƶԱȳ�ʱ,ʶ��ɹ���ָ�Ƶ���������ƾͲ�ʶ����
	if(Match_l_n(&mg200_ID[mg200_i])==0x00 && mg200_i<9){
		//�洢ʶ��ɹ���ID�б�λ�������ƶ�
		mg200_i++;
		//��ʾ�Աȳɹ���ID���,����ת�ַ�
		LCD_dis_ch(x,80,mg200_ID[mg200_i-1]+48,LIGHTBLUE,0,0xff,32);
		x+=20;
	}

	//ʶ������Ҫ���ƣ���Ϊ������ֻ��9��ָ�ƣ�����mg200���ܲ�ֹ9��
	if(mg200_i == 9){
		//������ʾһ�Σ���ɾ�������������
		mg200_i++;
		printf("ָ��ʶ������ﵽ����,����ɾ���Ѿ�ʶ��ָ��ID\r\n");
	}
	
	
	
	
	/*����ȷ��*/
	if(key == '*' )     //ȷ��ɾ�� �� �ص��ϼ��˵�
	{
		//����һ�������¼��ɾ����ID,��ʼ��Ϊ0,����Ϊ�˱�֤ɾ����ʱ�򣬲����ظ�ɾ����ֻ��ɾ��ʱʹ��
		u8 deleted_ids[9] = {0};  //�����9��ָ��ID��ɾ��
		u8 deleted_count = 0;
		//������ģ������˫ɾ��
			//ģ��,��0������mg200_i�����Ƕ����һ��
			for(int j=0;j<mg200_i;j++){
				//��鵱ǰID�Ƿ��Ѿ���ɾ����,��һ��ʶ��ɹ���ID���ʱ����־������
				u8 already_deleted = 0;
				for(int i = 0; i < 9; i++) {
					if(deleted_ids[i] == mg200_ID[j]) {
						//���ID�Ѿ���ɾ������
						already_deleted = 1;
						//�������У��
						break;
					}
				}

				//���IDû�б�ɾ����,�����ɾ������
				if(!already_deleted){
					//�����б��е�ָ��ID��ɾ��
					EraseOne(mg200_ID[j]);

					//��������ID���ݽ���ɾ��
					for(int k=0;k<9;k++){
						if(mg200_id[k] == mg200_ID[j]){
							//�ѱ������ݸ���Ϊ��Чֵ0xff
							mg200_id[k] = 0xff;
						}
					}

					//��¼��ɾ����ID
            		deleted_ids[deleted_count] = mg200_ID[j];
					deleted_count++;
				}
			}
		//����������д��AT24
		AT24C0x_write_bytes(21,sizeof(mg200_id),mg200_id,AT24C04);
		//��������
		voice(SETTING_SUCCESS);

		//���ϼ��˵�
		page_mode = 4;
		ad_flag=1;
	}
	
	
	else if(key == '#')   //ȡ���ᵽ�ϼ��˵�
	{
		page_mode = 4;
		ad_flag=1;
	}
}



/***********************************************
*������    :rfid_page
*��������  :��Ƶ��������溯��
*��������  :u8 key
*��������ֵ:��
*��������  :page_mode = 5
************************************************/
void rfid_page(u8 key)
{
	static u8 ad_flag = 1;
	//�״ν��뿨Ƭ��������־
	u8 open_flag;
	
	//ִ��һ��
	if(ad_flag)
	{
		//������λ����ʾ
		printf("\r\n��Ƭ�������:\r\n");
		printf("��1��ע�ῨƬ\r\n");
		printf("��2��ɾ��ָ��ID��Ƭ\r\n");
		printf("��3��ɾ�����п�Ƭ\r\n");
		printf("��4��ʶ��ɾ����Ƭ\r\n");
		printf("��#���ص���һ���˵�\r\n");
		printf("��*���ص�������\r\n");
		
		//LCD��Ļ����
		LCD_dis_pic(0,0,gImage_systemPic);
		LCD_dis(26,48,"��Ƶ������",LIGHTBLUE,0,0xff,32);
		LCD_dis(5,83,"��1��ע�ῨƬ",LIGHTBLUE,0,0xff,16);
		LCD_dis(5,108,"��2��ɾ��ָ����Ƭ",LIGHTBLUE,0,0xff,16);
		LCD_dis(5,133,"��3��ɾ�����п�Ƭ",LIGHTBLUE,0,0xff,16);
		LCD_dis(5,158,"��4��ʶ��ɾ����Ƭ",LIGHTBLUE,0,0xff,16);	
		LCD_dis(5,220,"�����桾*��",LIGHTBLUE,0,0xff,16);
		LCD_dis(145,220,"��#����һ��",LIGHTBLUE,0,0xff,16);
		
		
		//�ж��ǲ��ǵ�һ�ν��뿨Ƭ������棬�����Ƿ�9����Ƭ��UID��ά����ĳ�ʼֵд�뵽AT24C04��
		//10�ŵ�ַ���״ο�����־��20�ŵ�ַ���״δ�ָ�ƹ�����棬21~29�ŵ�ַ��ָ��ID��Ϣ
		//�״ν��뿨Ƭ��������־���Դ���30�ŵ�ַ��������ַ�濨ƬUID��Ϣ��ά����[9][4]
		at24c0x_read_byte(30,&open_flag);
		if(open_flag != OPEN_FLAG)
		{
			//��ԭʼ����洢��AT24��,ʹ��0xff��Ϊ��ʼ��Чֵ
			memset(picc,0xff,sizeof(picc));
			AT24C0x_write_bytes(31,sizeof(picc),picc,AT24C04);
			//�ѵ�һ�ν����־λд�뵽AT(��������AT24C04)
			at24c0x_write_byte(30,OPEN_FLAG);
		}

		ad_flag = 0;
	}
	
	
	switch(key)
	{   case '1':page_mode = 51;ad_flag=1;break;
        case '2':page_mode = 52;ad_flag=1;break;
        case '3':page_mode = 53;ad_flag=1;break;
        case '4':page_mode = 54;ad_flag=1;break;
		case '#':page_mode = 2;ad_flag=1;break;
		case '*':page_mode = 1;ad_flag=1;break;	
	}
}




/***********************************************
*������    :picc_user_page
*��������  :ע���û���Ƭ���溯��
*��������  :u8 key
*��������ֵ:��
*��������  :page_mode = 51
************************************************/
void picc_user_page(u8 key)
{
	static u8 ad_flag = 1;
	u8 picc_i,picc_j;
	//��ӡ��ƬID�������
	static u16 x;
	//�û��Ѿ�ʹ��ID����
	static u8 picc_count = 0;
	u8 sta;
	//�Ƿ��Ѿ�ע��ı�ʶλ
	u8 login_flag = 1;

	
	//ִ��һ��
	if(ad_flag)
	{
		//������λ����ʾ
		printf("\r\nע���û���Ƭ����:\r\n");
		printf("��#���ص���һ���˵�\r\n");
		printf("��*���ص�������\r\n");
		
		
		//LCD��Ļ��ʾ
		LCD_dis_pic(0,0,gImage_systemPic);
		LCD_dis(24,48,"ע���û���Ƭ",LIGHTBLUE,0,0xff,32);
		LCD_dis_number_pic(50,120,"P",LGRAY,1,gImage_systemPic);
		LCD_dis_number_pic(130,120,"Q",LGRAY,1,gImage_systemPic);
		LCD_dis(0,220,"��*��������",LIGHTBLUE,0,0xff,16);
		LCD_dis(150,220,"��һ����#��",LIGHTBLUE,0,0xff,16);


		//��AT24�ж�����ƬID���ݵ���ά������
		AT24C0x_read_bytes(31,sizeof(picc),picc);
		x = 30;
		picc_count = 0;
		//������ά����ȡ����Ч��Ƭid����ӡ�Ѿ�ע��Ŀ�ƬID���
		for(picc_i=0;picc_i<9;picc_i++)
		{
			for(picc_j=0;picc_j<4;picc_j++)
			{
				//ֻҪ�п�ƬUID���ݣ���ô��Ӧ���9��Ԫ���е��ĸ�СԪ�ز�����ȫ��0xff
				//Ĭ����û�и�ֵ��0xff,����ÿһ���û�ID�µĿ�ƬUID���ĸ��ֽ�ֻҪ��һ������0xff,�ʹ�������û�ID�п�Ƭ��
				//������û�ID�����
				if(picc[picc_i][picc_j] != 0xff)
				{	
					//��ʾ��ƬID���1~9��ͼƬ��ʾ��Ҫת��һ�£���Ȼ��ʾ����0~8
					LCD_dis_ch(x,80,picc_i+48+1,LIGHTBLUE,0,0xff,32);
					picc_count++;
					x+=20;
					break;
				}
			}
		}
		//�Ѿ�����9����Ƭ�������������һ������������ʾ
		if(picc_count == 9)
		{
			LCD_dis(30,170,"ע���Ѵ�����",RED,0,0xff,32);
			
		}else {
			//ÿһ��ˢ�½�����û�г��������Ͳ���
			voice(PUTCARD);
		}

		ad_flag = 0;
	}


	//���������ע�ᣬ���ҵ�Ҫע�ῨƬID���ݶ�Ӧ�Ķ�ά�����±�
	if(picc_count<9)
	{
		//picc_id��ֵ�ɺ������ݳ��������Ƕ����õ��Ŀ�Ƭ���кţ����������ǰ�棬��Ȼ�ò�����Ƭ���к�
		sta = WriteCardData(62,picc_data,picc_id);
		if(sta == MI_OK)
		{
			//�ȱ�����ά���ݣ����˿�Ƭ��û���ڱ���ע���������Ѿ�ע������Ͳ�����ע����
			for(picc_i=0;picc_i<9;picc_i++)
			{
				//���кŶԱȳɹ����ֽ�����
				u8 temp_count = 0;
				for(picc_j=0;picc_j<4;picc_j++)
				{
					//���жϴ˿�Ƭ��û���ڱ���ע���
					if(picc[picc_i][picc_j] == picc_id[picc_j])
					{
						//��Ƭ���ĸ��ֽ����ȫ����ȣ�˵���Ѿ�ע���������ѭ��
						temp_count++;
					}
				}

				//����ĸ��ֽڶ���ȣ�˵���Ѿ�ע���������ѭ��
				if(temp_count == 4)
				{
					//����ѭ��,��������,�������趼����ִ����
					login_flag = 0;
					printf("�˿�Ƭ�Ѿ�ע�ᣡ��Ƭ���������Ѿ�д�룬���Ǳ�ϵͳû�м�¼\r\n");
					voice(CARD_REGISTERED);
					break;
				}
			}

			if(login_flag == 1){
				//����������õ�ID�±�
				for(picc_i=0;picc_i<9;picc_i++)
				{
					for(picc_j=0;picc_j<4;picc_j++)
					{	
						//�����һ������0xff�ʹ������ID�ռ䱻ʹ����
						if(picc[picc_i][picc_j] != 0xff)
						{
							//�����ڲ�ѭ��������һ��ѭ��
							break;
						}
					}
					//һ���ռ��ĸ�СԪ�ض���0xff�Ļ��ʹ������ʹ�ã�����ѭ���ҵ�һ�����ÿռ�
					if(picc_j == 4)
					{
						//�������ѭ��
						break;
					}
					
				}

				//picc_i����Ҫ�洢��Ԫ���±꣬û�ж�����ѭ���У������±������
				//ע��������һ�ν���ע�ắ��ʱ���±���������¶���
				for(picc_j=0;picc_j<4;picc_j++)
				{
					//�����õ����ĸ��ֽڵĿ�Ƭ���кţ��洢��������
					picc[picc_i][picc_j] = picc_id[picc_j];
				}

				//���ע�������ﵽ���ޣ���������ˢ��ҳ��ʱ��ʾ

				//�����û��Ŀ�Ƭ��Ŵ���AT24C0x
				AT24C0x_write_bytes(31,sizeof(picc),picc,AT24C04);
				voice(SETTING_SUCCESS);
				//����ˢ�¸ý����ӡ���û�ID���
				ad_flag = 1;
			}
		}
	}



	
	switch(key)
	{
		case '#':page_mode = 5;ad_flag=1;break;
		case '*':page_mode = 1;ad_flag=1;break;
	}
}



/***********************************************
*������    :erase_user_picc_page
*��������  :ɾ��ָ����Ƭ���溯��
*��������  :u8 key
*��������ֵ:��
*��������  :page_mode = 52,��νɾ�����ǰѱ�����¼ɾ�����ɣ���Ϊ���ŶԱ���Ҫ������¼
************************************************/
void erase_user_picc_page(u8 key)
{
	static u8 ad_flag = 1;
	u8 picc_i,picc_j;
	//��ӡ��ƬID�������
	static u16 x;
	//�û��Ѿ�ʹ��ID����
	static u8 picc_count = 0;


	//ִ��һ��
	if(ad_flag)
	{
		//������λ����ʾ
		printf("\r\nɾ��ָ����Ƭ����:\r\n");
		printf("��ѡ��Ƭ\r\n");
		printf("��#���ص���һ���˵�\r\n");
		printf("��*���ص�������\r\n");

		//LCD��Ļ��ʾ
		LCD_dis_pic(0,0,gImage_systemPic);
		LCD_dis(24,48,"ɾ��ָ����Ƭ",LIGHTBLUE,0,0xff,32);
		LCD_dis(0,220,"��*��������",LIGHTBLUE,0,0xff,16);
		LCD_dis(150,220,"��һ����#��",LIGHTBLUE,0,0xff,16);
		voice(DELETE_ASSIGN_CARD);


		//��AT24�ж�����ƬID���ݵ���ά������
		AT24C0x_read_bytes(31,sizeof(picc),picc);
		x = 30;
		picc_count = 0;


		//������ά����ȡ����Ч��Ƭid����ӡ�Ѿ�ע��Ŀ�ƬID���
		for(picc_i=0;picc_i<9;picc_i++)
		{
			for(picc_j=0;picc_j<4;picc_j++)
			{
				//ֻҪ�п�ƬUID���ݣ���ô��Ӧ���9��Ԫ���е��ĸ�СԪ�ز�����ȫ��0xff
				//Ĭ����û�и�ֵ��0xff,����ÿһ���û�ID�µĿ�ƬUID���ĸ��ֽ�ֻҪ��һ������0xff,�ʹ�������û�ID�п�Ƭ��
				//������û�ID�����
				if(picc[picc_i][picc_j] != 0xff)
				{	
					//��ʾ��ƬID���1~9��ͼƬ��ʾ��Ҫת��һ�£���Ȼ��ʾ����0~8
					LCD_dis_ch(x,80,picc_i+48+1,LIGHTBLUE,0,0xff,32);
					picc_count++;
					x+=20;
					break;
				}
			}
		}
		
		ad_flag = 0;
	}

	//��ɾ����ǰ�������û���Ƭע���¼
	if(picc_count>0){
		//ɾ��ָ��ID�û��Ŀ�Ƭ��keyΪ��Чֵ��ִ��ɾ������
		if(key != 0xff && key != '#' && key != '*'){
			//����ֵ���û�ID�洢λ�õ�ת������ĳһ���û�ID�µĿ�Ƭ��¼ɾ��
			for(int i=0;i<4;i++){
				picc[key-48-1][i] = 0xff;
			}
			//�ٴ���AT24C0x��
			AT24C0x_write_bytes(31,sizeof(picc),picc,AT24C04);
			voice(SETTING_SUCCESS);
			picc_count--;

			//ˢ��ҳ���ӡ�µ���ЧID
			ad_flag = 1;
		}

	}else {
		printf("û���û�ע���¼,����ע�ῨƬ\r\n");
		LCD_dis(30,170,"����ע�ῨƬ",RED,0,0xff,32);
	}
	
	
	
	switch(key)
	{
		case '#':page_mode = 5;ad_flag=1;break;
		case '*':page_mode = 1;ad_flag=1;break;
	}
	
}



/***********************************************
*������    :erase_picc_all_page
*��������  :ɾ�����п�Ƭ���溯��
*��������  :u8 key
*��������ֵ:��
*��������  :page_mode = 53
************************************************/
void erase_picc_all_page(u8 key)
{
	static u8 ad_flag = 1;


	//ִ��һ��
	if(ad_flag)
	{
		//������λ����ʾ
		printf("\r\nɾ�����п�Ƭ?:\r\n");
		printf("��*��ȷ��\r\n");
		printf("��#��ȡ��\r\n");
		
		
		//LCD��Ļ��ʾ
		LCD_dis_pic(0,0,gImage_systemPic);
		LCD_dis(24,48,"ɾ�����п�Ƭ",LIGHTBLUE,0,0xff,32);
		LCD_dis(56,90,"ȷ��ɾ��",RED,0,0xff,32);
		LCD_dis_number_pic(96,126,"?",RED,1,gImage_systemPic);
		LCD_dis(0,220,"ȷ��(*)",LIGHTBLUE,0,0xff,16);
		LCD_dis(180,220,"ȡ��(#)",LIGHTBLUE,0,0xff,16);
		voice(DELETE_ALL_CARD);

		
		ad_flag = 0;
	}
	
	
	
	
	if(key == '*')     //ȷ��ɾ�� �� �ص��ϼ��˵�
	{
		//ɾ���Ķ���,���ǰѱ�����¼ȫ���������
		memset(picc,0xff,sizeof(picc));
		//�ٴ���AT24C0x��
		AT24C0x_write_bytes(31,sizeof(picc),picc,AT24C04);
		voice(SETTING_SUCCESS);
		
		//���ϼ��˵�
		page_mode = 5;
		ad_flag=1;
	}
	
	else if(key == '#')   //ȡ���ᵽ�ϼ��˵�
	{
		page_mode = 5;
		ad_flag=1;
	}

}


/***********************************************
*������    :erase_picc_match_page
*��������  :ʶ��ɾ����Ƭ���溯��
*��������  :u8 key
*��������ֵ:��
*��������  :page_mode = 54
************************************************/
void erase_picc_match_page(u8 key)
{
	static u8 ad_flag = 1;
	//��ӡ��ƬID�������
	static u8 x;
	//Ѱ��������״̬
	u8 sta;
	u8 picc_i,picc_j;
	//�û��Ѿ�ʶ��ɹ���ID����
	static u8 picc_count = 0;
	//��¼�Ѿ�ʶ��Ŀ�ƬID
	static u8 picc_idNum[9] = {0};
	//�Ƿ��Ѿ��ﵽʶ�����޵ı�ʶλ
	u8 match_flag = 1;
	//�Ƿ��Ѿ�����ʶ���б��־
	u8 sava_flag = 1;
	//���кŶԱȳɹ����ֽ�����
	u8 temp_count = 0;

	
	//ִ��һ��
	if(ad_flag)
	{
		//������λ����ʾ
		printf("\r\nʶ��ɾ����Ƭ����\r\n");
		printf("ʶ��Ƭ\r\n");
		printf("ȷ��(*)\r\n");
		printf("����(#)\r\n");
		
		//LCD��Ļ��ʾ
		LCD_dis_pic(0,0,gImage_systemPic);//����ͼƬΪ����
		LCD_dis(25,48,(u8 *)"ʶ��ɾ����Ƭ",LIGHTBLUE,0,0xff,32);
		LCD_dis_number_pic(50,120,"P",LGRAY,1,gImage_systemPic);
		LCD_dis_number_pic(130,120,"Q",LGRAY,1,gImage_systemPic);
		LCD_dis(0,220,"ȷ��(*)",LIGHTBLUE,0,0xff,16);
		LCD_dis(180,220,(u8 *)"����(#)",LIGHTBLUE,0,0xff,16);
		voice(PUTCARD);
		

		//��AT24�ж�����ƬID���ݵ���ά������
		AT24C0x_read_bytes(31,sizeof(picc),picc);
		x = 30;
		picc_count = 0;
		//��ʶ���б����³�ʼ����Чֵ���ȴ�����ʶ��
		//ʶ��һ���ȥ�ˣ��ٽ�����Ҫ����ʶ��
		for(int l=0;l<9;l++){
			picc_idNum[l] = 0;
		}

		ad_flag = 0;
	}


	//ʶ��ɹ��������Ѿ��ﵽ����9����������ʶ��
	if(picc_count==9){
		printf("ʶ���Ѵ�����,����ɾ��\r\n");
		match_flag = 0;
	}

	if(match_flag==1){
		sta = MatchCard(picc_id);
		if(sta == MI_OK){
			//��ʱ�Ѿ��ҵ���Ƭ���Ѿ����������кţ���ʼ������ά�����ѯ�û���Ӧ�Ŀ�ƬID
			for(picc_i=0;picc_i<9;picc_i++)
			{
				//���кŶԱȳɹ����ֽ�����
				temp_count = 0;
				for(picc_j=0;picc_j<4;picc_j++)
				{
					//���жϴ˿�Ƭ
					if(picc[picc_i][picc_j] == picc_id[picc_j])
					{
						//��Ƭ���ĸ��ֽ����ȫ����ȣ�˵���Ѿ�ע���
						temp_count++;
					}
				}

				//����ĸ��ֽڶ���ȣ�˵���Ѿ�ע���������ѭ��
				if(temp_count == 4)
				{
					//����ѭ��,��������,�������趼����ִ����
					printf("�˿�Ƭ��ע��IDΪ%d\r\n",picc_i+1);
					break;
				}
			}
			//�˿�Ƭע�������ʾ
			if(temp_count == 4){
				//���ж�֮ǰ��û�д����ʶ���б��У����֮ǰ�У��Ͳ����ٴ���
				//��ЧֵҲ�����������
				for(int i=0;i<sizeof(picc_idNum);i++){
					if(picc_idNum[i] == (picc_i+1)){
						sava_flag = 0;
					}
				}
				//֮ǰû�д�����Ŵ��ȥ
				if(sava_flag == 1){
					//��Ļ��ʾ�ҵ��Ŀ�Ƭע���ID
					//��ʾ��ƬID���1~9��ͼƬ��ʾ��Ҫת��һ�£���Ȼ��ʾ����0~8
					LCD_dis_ch(x,80,picc_i+48+1,LIGHTBLUE,0,0xff,32);
					//ʶ������+1,������ʱ������,��ID�����ȥ
					picc_idNum[picc_count] = picc_i+1;
					picc_count++;
					x+=20;
					voice(Di);
				}
			}else {
				printf("�˿�Ƭδע��,����ע��\r\n");
			}
			
		}
	}
	
	

	
	if(key == '*')     //ȷ��ɾ�� �� �ص��ϼ��˵�
	{
		//�������ʶ���б�ʱ�Ѿ����˷��ظ�������û��Ҫ�����Ҳ�����ˣ���
		//ɾ���Ķ���,�ӻ������õ������Ѿ�ʶ�𵽵�id��Ȼ��ɾ����ά�����е����ݣ��ٴ���AT24C0x��
		//����һ�������¼��ɾ����ID,��ʼ��Ϊ0,����Ϊ�˱�֤ɾ����ʱ�򣬲����ظ�ɾ����ֻ��ɾ��ʱʹ��
		u8 deleted_ids[9] = {0};  //�����9����ƬID��ɾ��1~9
		u8 deleted_count = 0;
		//����������ʶ��ɹ��Ŀ�ƬID��ȥ��ά������Ӧλ��ɾ������
		for(int j=0;j<9;j++){
			//��鵱ǰID�Ƿ��Ѿ���ɾ����,��һ��ʶ��ɹ���ID���ʱ����־������
			u8 already_deleted = 0;
			for(int i = 0; i < 9; i++) {
				//����û��ʶ��ֱ�Ӱ�ɾ������ô����ȣ�Ҳ����ɾ��
				//ʶ������û���������µ�ID��Ϊ0��Ҳ����ɾ��
				if(deleted_ids[i] == picc_idNum[j]) {
					//���ID�Ѿ���ɾ������
					already_deleted = 1;
					//�������У��
					break;
				}
			}

			//���IDû�б�ɾ����,�����ɾ������
			if(!already_deleted){
				//���û�б�ɾ����,ȥ��ά�����ж�Ӧλ������Чֵ
				for(int i=0;i<4;i++){
					picc[picc_idNum[j]-1][i] = 0xff;
				}

				//��¼��ɾ����ID
				deleted_ids[deleted_count] = picc_idNum[j];
				deleted_count++;
			}
		}

		//��ά����ȫ���޸ĺú��ٴ���AT24C0x��
		//û��ʶ��ֱ�Ӱ��Ļ������Ƕ�������û���޸ľʹ���
		AT24C0x_write_bytes(31,sizeof(picc),picc,AT24C04);
		voice(SETTING_SUCCESS);
		

		//���ϼ��˵�
		page_mode = 5;
		ad_flag=1;
	}
	
	else if(key == '#')   //ȡ���ᵽ�ϼ��˵�
	{
		page_mode = 5;
		ad_flag=1;
	}

	
}



/***********************************************
*������    :voice_light_page
*��������  :��������Ļ���ȹ�����溯��
*��������  :u8 key
*��������ֵ:��
*��������  :page_mode = 6
************************************************/
void voice_light_page(u8 key)
{
	static u8 ad_flag = 1;
	
	
	//ִ��һ��
	if(ad_flag)
	{
		//������λ����ʾ
		printf("\r\n�������ȵ��ڽ���:\r\n");
		printf("��1��������\r\n");
		printf("��2��������\r\n");
		printf("��3������ģʽ\r\n");
		printf("��4����Ļ���ȼ�\r\n");
		printf("��5����Ļ���ȼ�\r\n");
		printf("��#���ص���һ���˵�\r\n");
		printf("��*���ص�������\r\n");
		
		//LCD��Ļ��ʾ
		LCD_dis_pic(0,0,gImage_systemPic);
		LCD_dis(24,48,"�������ȵ���",LIGHTBLUE,0,0xff,32);
		LCD_dis_number_pic(5,80,"S",LGRAY,1,gImage_systemPic);
		LCD_dis_number_pic(5,140,"L",LGRAY,1,gImage_systemPic);
		
		LCD_dis(0,220,"��*��������",LIGHTBLUE,0,0xff,16);
		LCD_dis(150,220,"��һ����#��",LIGHTBLUE,0,0xff,16);
		
		ad_flag = 0;
	}
	
	
	//������һ���˵�
	if(key=='#')
	{
		page_mode = 2;       //���ع���Ա����
		ad_flag = 1;
	}
	//����������
	else if(key=='*')
	{
		page_mode = 1;       //����������
		ad_flag = 1;
	}
	
	//������
	else if(key=='1')
	{
		
	}
	//������
	else if(key=='2')
	{
			
	}
	//����
	else if(key=='3')
	{
		
	}
	
	
	//������
	else if(key == '4')
	{
		
	}

	//������
	else if(key == '5')
	{
		
	}
}

/***********************************************
*������    :factory_reset_page
*��������  :�ָ��������ý��溯��
*��������  :u8 key
*��������ֵ:��
*��������  :page_mode = 7
************************************************/
void factory_reset_page(u8 key)
{
	static u8 ad_flag = 1;
	
	//ִ��һ��
	if(ad_flag)
	{
		//������λ����ʾ
		printf("\r\n�ָ��������ý���:\r\n");
		printf("��*��ȷ��\r\n");
		printf("��#��ȡ��\r\n");
		
		//LCD��Ļ��ʾ
		LCD_dis_pic(0,0,gImage_systemPic);
		LCD_dis(24,48,"�ָ���������",LIGHTBLUE,0,0xff,32);
		LCD_dis(56,85,"ȷ�ϻָ���",LIGHTBLUE,0,0xff,16);
		LCD_dis_number_pic(96,126,"H",RED,1,gImage_systemPic);
		LCD_dis(0,210,"ȷ��(*)",LIGHTBLUE,0,0xff,16);
		LCD_dis(180,210,"ȡ��(#)",LIGHTBLUE,0,0xff,16);
		
		
		ad_flag = 0;
	}
	
	//ȷ��
	if(key=='*')
	{
		page_mode = 1;       //��������
		ad_flag = 1;
	}
	//����
	else if(key=='#')
	{
		page_mode = 2;       //��һ���˵�
		ad_flag = 1;
	}
}






























