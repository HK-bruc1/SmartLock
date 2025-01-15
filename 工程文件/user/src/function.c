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
    u8 sta;
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



    /*���������¼������,������������ʱ��ֻ֤����һ�Σ�������Ч��������CPUЧ���½�*/
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
    }




    /*�ж��ǵ�һ�ο�������Ҫ��������*/
    if(open_val != OPEN_FLAG){
        //�����һ�γ�ʼ�������������
        if(pass_flag==1){
            LCD_dis(70,130,"������������",RED,0,RED,16);
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
                LCD_dis(70,130,"���ٴ�����������",RED,0,RED,16);
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
}