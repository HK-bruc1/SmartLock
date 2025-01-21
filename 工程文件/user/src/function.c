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
    //���뿪����״̬
    u8 sta;
    //���뿪��
    sta = open_passward(key);
    if(sta == 1){
        //���ں����в����ý�����һ��ҳ�棬��while�в���ͨ����־λ��ѯ������Ӧ���漴��
        page_mode = 2;
    }



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
	//��ʾ����
	static u16 x = 30;
	u8 mg200_i;
	//Ҫ¼���ָ��ID
	static u8 mg200_ID;
	
	//ִ��һ��
	if(ad_flag)
	{
		//��������
		voice(REGISTER_FINGER);
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
			//��ʾ��ע���ID��IDתASCII�ַ�
			LCD_dis_ch(x,50,mg200_ID+48,GREEN,32,0,NULL);
			x+=20;

			//�ͷ�״̬��������ˢ�µ�ǰҳ�棬��ӡ�µ�ID
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
	
	
	//ִ��һ��
	if(ad_flag)
	{
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
		
		
		ad_flag = 0;
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
		//������λ����ʾ
		printf("\r\nɾ������ָ�ƣ�:\r\n");
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
	
	//ִ��һ��
	if(ad_flag == 1)
	{
		//������λ����ʾ
		printf("\r\nʶ��ɾ��ָ�ƽ���\r\n");
		printf("ʶ��ָ��\r\n");
		printf("��*��ȷ��\r\n");
		printf("��#��ȡ��\r\n");
		
		//LCD��Ļ��ʾ
		LCD_dis_pic(0,0,gImage_systemPic);//����ͼƬΪ����
		LCD_dis(25,48,(u8 *)"ʶ��ɾ��ָ��",LIGHTBLUE,0,0xff,32);
		LCD_dis_number_pic(96,140,(u8 *)"Z",LGRAY,1,gImage_systemPic);
		LCD_dis(160,220,"��#������",LIGHTBLUE,0,0xff,16);
		LCD_dis(0,220,"ȷ�ϡ�*��",LIGHTBLUE,0,0xff,16);
		
		
		ad_flag = 0;
	}
	
	
	
	
	/*����ȷ��*/
	if(key == '*' )     //ȷ��ɾ�� �� �ص��ϼ��˵�
	{
		
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
	
	//ִ��һ��
	if(ad_flag)
	{
		//������λ����ʾ
		printf("\r\n��Ƭ������棺\r\n");
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
	
	//ִ��һ��
	if(ad_flag)
	{
		//������λ����ʾ
		printf("\r\nע���û���Ƭ���棺\r\n");
		printf("��#���ص���һ���˵�\r\n");
		printf("��*���ص�������\r\n");
		
		
		//LCD��Ļ��ʾ
		LCD_dis_pic(0,0,gImage_systemPic);
		LCD_dis(24,48,"ע���û���Ƭ",LIGHTBLUE,0,0xff,32);
		LCD_dis_number_pic(50,120,"P",LGRAY,1,gImage_systemPic);
		LCD_dis_number_pic(130,120,"Q",LGRAY,1,gImage_systemPic);
		LCD_dis(0,220,"��*��������",LIGHTBLUE,0,0xff,16);
		LCD_dis(150,220,"��һ����#��",LIGHTBLUE,0,0xff,16);
		
		ad_flag = 0;
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
*��������  :page_mode = 52
************************************************/
void erase_user_picc_page(u8 key)
{
	static u8 ad_flag = 1;
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
		
		

		ad_flag = 0;
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
		LCD_dis(56,85,"ȷ��ɾ��",LIGHTBLUE,0,0xff,16);
		LCD_dis_number_pic(96,126,"?",RED,1,gImage_systemPic);
		LCD_dis(0,220,"ȷ��(*)",LIGHTBLUE,0,0xff,16);
		LCD_dis(180,220,"ȡ��(#)",LIGHTBLUE,0,0xff,16);
		
		ad_flag = 0;
	}
	
	
	
	
	if(key == '*')     //ȷ��ɾ�� �� �ص��ϼ��˵�
	{
		//ɾ���Ķ���

		
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
	
	//ִ��һ��
	if(ad_flag)
	{
		//������λ����ʾ
		printf("\r\nʶ��ɾ����Ƭ����\r\n");
		printf("ʶ��Ƭ\r\n");
		printf("��#��ȡ��\r\n");
		
		//LCD��Ļ��ʾ
		LCD_dis_pic(0,0,gImage_systemPic);//����ͼƬΪ����
		LCD_dis(25,48,(u8 *)"ʶ��ɾ����Ƭ",LIGHTBLUE,0,0xff,32);
		LCD_dis_number_pic(50,120,"P",LGRAY,1,gImage_systemPic);
		LCD_dis_number_pic(130,120,"Q",LGRAY,1,gImage_systemPic);
		LCD_dis(160,220,(u8 *)"��#������",LIGHTBLUE,0,0xff,16);
		
		
		ad_flag = 0;
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






























