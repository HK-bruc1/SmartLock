#include "main.h"

//各模块整合的综合性函数


//字库更新函数，放在main函数前面，这样的话，不能随变使用按键2就能更新字库，只能在重启时按住2才能更新字库，作为一个特殊功能使用
void zk_update(void){
    //获取按键值
    u8 key = key_scan();
    //判断按键值
    if(key == 2){
        printf("正在擦除字库存储所需要的空间...");
        //0x 7f  f  f  ff
        //   区  块 页  地址
        w25q64_blocks(ZK_ADDR,21);
        printf("擦除完成！\n");
        //开启字库更新标志
        zk_flag = 1;
    }
}



/***********************************************
*函数名    :open_passward
*函数功能  :密码开锁
*函数参数  :u8 bs8116_key
*函数返回值:u8
*函数描述  :返回1 开门成功
           返回0 开门失败
************************************************/
u8 open_passward(u8 bs8116_key){
    //密码解锁的状态返回值
    u8 sta;
    //第一次开机标志位的值,由于只会获取一次但是会进入多次，需要保存下来，不然第二次就会出错
    static u8 open_val;
    //密码登录页面状态锁
    static u8 pass_page_flag = 1;
    //密码开锁的界面圆点坐标
    static u8 x = 45,y = 200;
    //设置密码的语言播报状态锁
    static u8 pass_flag = 1;
    //用于存储两次密码的值
	static u8 pass_word_1[7] = {0};
	static u8 pass_word_2[7] = {0};
    //用于计数密码个数画圆的个数
	static u8 pass_i = 0;



    /*公用密码登录主界面,输入密码流程时保证只进入一次，避免无效操作导致CPU效率下降*/
    if(pass_page_flag){
        //读第一次开机标志位
        at24c0x_read_byte(10,&open_val);
        //密码开锁界面
        LCD_dis_pic(0,0,gImage_systemPic);
        x=45;
        for(u8 i=0;i<6;i++)
		{
			LCD_Ring(x,y,8,LGRAY);
			x+=30;
		}
        
        //锁住主界面，防止反复刷新
        pass_page_flag = 0;
        //坐标归位，方便画实心圆
        x=45;
    }




    /*判断是第一次开机，需要设置密码*/
    if(open_val != OPEN_FLAG){
        //输入第一次初始密码的语音播报
        if(pass_flag==1){
            LCD_dis(70,130,"请设置新密码",RED,0,RED,16);
            voice(INPUT_NEW_PASSWORD);
            //锁住设置密码的语音播报，防止反复播报
            pass_flag=0;
        }

        //只拿有效密码值
        if(bs8116_key !='*' && bs8116_key!='#' && bs8116_key!=0xff){
            //第一次输入密码
            if(pass_i<6){
                //数组层面
                pass_word_1[pass_i] = bs8116_key;
                pass_i++;
                //屏幕跟上实心圆
                LCD_shiRing(x,y,8,LGRAY);
                //圆心坐标移动
                x+=30;

                //pass_i到6就表示第一次密码输入完成
                if(pass_i==6){
                //把实心圆清除重新填充背景图片像素颜色值
                LCD_xy_clear(45-8,200-8,166,16,0xffff,1,gImage_systemPic);
                //重新换空心圆
                x=45;
                for(u8 i=0;i<6;i++)
                {
                    LCD_Ring(x,y,8,LGRAY);
                    x+=30;
                }

                //在显示之前把文字需要的背景先清除干净：把文字所在的区域背景像素值全部改成对应图片颜色值
                LCD_xy_clear(70,130,100,16,0xffff,1,gImage_systemPic);
                LCD_dis(70,130,"请再次输入新密码",RED,0,RED,16);
                voice(INPUT_NEW_PASSWORD_AGAIN);
                //圆心归位，以便下一次使用
                x=45;
                
                }
            }
            //第二次输入密码
            else if(pass_i>=6){
                pass_word_2[pass_i-6] = bs8116_key;
                pass_i++;
                //屏幕实心圆再次跟上
                LCD_shiRing(x,y,8,LGRAY);
                //圆心坐标移动
                x+=30;

                //当pass_i到12时，就表示第二次密码输入完成
                if(pass_i==12){
                    //对比两次密码是否一致
                    if(strcmp((char *)pass_word_1,(char *)pass_word_2)==0){
                        //将密码存入at24
                        AT24C0x_write_bytes(11,sizeof(pass_word_2),pass_word_2,AT24C04);
                        //写入第一次开机标志位
                        at24c0x_write_byte(10,OPEN_FLAG);

                        //回到公用密码登录界面,重新刷新图片和空心圆,把所有状态锁打开
                        pass_page_flag = 1;
                        //把密码输入个数记录清除
                        pass_i = 0;
                        //打开首次开机的语音播报状态锁,因为设置初始密码后续就不进入第一次开机逻辑了
                        //但是正常密码解锁逻辑也需要，需要还是解锁
                        pass_flag = 1;
                        voice(SETTING_SUCCESS);
                    }
                    else{
                        //回到公用密码登录界面,重新刷新图片和空心圆,把所有状态锁打开
                        //重新开始密码输入操作
                        pass_flag = 1;//打开首次开机的语音播报状态锁，因为密码不一致需要重新播报
                        pass_i = 0;
                        pass_page_flag = 1;
                        voice(PASSWORD_INCONFORMITY);
                    }
                }
            } 
        }
    }  


    /*不是第一次开机，需要输入密码对比*/
    //经过第一次重新设置密码后，进入到正常密码解锁逻辑
    else{
        //正常密码解锁语音播报(只播报一次即可),pass_flag状态锁不是进入第一次开机逻辑后，这里也可以使用
        if(pass_flag==1){
            voice(INPUT_ADMIN_PASSWORD);
            pass_flag = 0;

        }
        //先获取用户输入的六位密码，此前pass_i的值已经归零
        if(bs8116_key !='*' && bs8116_key!='#' && bs8116_key!=0xff){
            //第一次输入密码
            if(pass_i<6){
                //数组层面
                pass_word_1[pass_i] = bs8116_key;
                pass_i++;
                //屏幕跟上实心圆
                LCD_shiRing(x,y,8,LGRAY);
                //圆心坐标移动
                x+=30;

                //pass_i到6就表示密码输入完成
                if(pass_i==6){
                    //提取密码对比
                    AT24C0x_read_bytes(11,sizeof(pass_word_2),pass_word_2);
                    if(strcmp((char *)pass_word_1,(char *)pass_word_2)==0){
                        voice(DOOROPEN_SUCCESS);
                        door_open();
                        //开门之后，把所有状态锁打开，回到密码登录界面
                        pass_page_flag = 1;
                        pass_i = 0;
                        pass_flag = 1;

                        sta = 1;
                    }
                    else {
                        //开门失败，回到密码登录界面
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