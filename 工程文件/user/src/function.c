#include "main.h"




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