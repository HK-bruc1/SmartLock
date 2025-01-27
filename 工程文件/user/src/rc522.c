#include "main.h"


/*****************辰哥单片机设计******************
											STM32
 * 文件			:	MFRC522射频模块c文件                   
 * 版本			: V1.0
 * 日期			: 2024.9.2
 * MCU			:	STM32F103C8T6
 * 接口			:	见代码							
 * BILIBILI	:	辰哥单片机设计
 * CSDN			:	辰哥单片机设计
 * 作者			:	辰哥

**********************BEGIN***********************/


/*********************************************************
*函数名    ：MFRC522_Init
*函数功能  ：RFID所用SPI的GPIO初始化配置以及本身硬件的GPIO口初始化
*函数参数  ：无
*函数返回值：无
*函数描述  ：
*              PA15--CS--推挽输出
*              PC10--SCL--推挽输出
*              PC11--MISO--浮空输入
*              PC12--MOSI--推挽输出
*              PD2--RST--推挽输出
*********************************************************/
void MFRC522_Init(void)
{
    //GPIO初始化
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC |RCC_AHB1Periph_GPIOD, ENABLE);//1.开时钟
	
	//PC10--SCL--推挽输出,PC12--MOSI--推挽输出
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	//PA15--CS--推挽输出
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	//PD2--RST--推挽输出
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOD, &GPIO_InitStruct);
	
	//PC11--MISO--浮空输入
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	//片选拉高，复位脚拉高
	GPIO_SetBits(GPIOD, GPIO_Pin_2);
	GPIO_SetBits(GPIOA,GPIO_Pin_15);
}


//发送一个字节（写RC522寄存器时会调用）
//时钟极性 1 高
//时钟相位 1 高  
u8 SPITransferByte(u8 data){
    u8 i;

    MFRC522_SCK_H;//空闲拉高

    for(i=0;i<8;i++){
        //发送一位数据
        MFRC522_SCK_L;
        if(data & 0x80)
            MFRC522_MOSI_H;
        else
            MFRC522_MOSI_L;
        data <<= 1;
        
        //接收一位数据
        MFRC522_SCK_H;
        if(MFRC522_MISO_READ)
            data |= 0x01;
    }
    return data;
}

	

/////////////////////////////////////////////////////////////////////
//功能描述?从MFRC522的某一寄存器读一个字节数据
//参数说明：Address[IN]:寄存器地址
//返    回：读出的值
/////////////////////////////////////////////////////////////////////
unsigned char Read_MFRC522(unsigned char Address)
{
    unsigned char ucAddr;
    unsigned char ucResult=0;

    MFRC522_SDA_L;
    RC522_DELAY();
    ucAddr = ((Address<<1)&0x7E)|0x80;

    SPITransferByte(ucAddr);
    ucResult = SPITransferByte(0xff);

    MFRC522_SDA_H;
    RC522_DELAY();
    return ucResult;
}



/////////////////////////////////////////////////////////////////////
//功能描述?向MFRC522的某一寄存器写一个字节数据
//参数说明：Address[IN]:寄存器地址
//          value[IN]:写入的值
/////////////////////////////////////////////////////////////////////
void Write_MFRC522(unsigned char Address, unsigned char value)
{  
    unsigned char ucAddr;

    MFRC522_SDA_L;
    ucAddr = ((Address<<1)&0x7E);

    SPITransferByte(ucAddr);
    SPITransferByte(value);
    

    MFRC522_SDA_H;
	RC522_DELAY();
}


/////////////////////////////////////////////////////////////////////
//功    能：复位RC522
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char MFRC522_Reset(void) 
{
	//unsigned char i;
    MFRC522_RST_H;
	RC522_DELAY();  

    MFRC522_RST_L;
	RC522_DELAY();  

    MFRC522_RST_H;
	RC522_DELAY();        	

    //MFRC522_RST_H;
    Write_MFRC522(CommandReg,0x0F); //soft reset
    while(Read_MFRC522(CommandReg) & 0x10); //wait chip start ok
    RC522_DELAY();            

    Write_MFRC522(ModeReg,0x3D);            //和Mifare卡通讯，CRC初始值0x6363
    Write_MFRC522(TReloadRegL,30);           
    Write_MFRC522(TReloadRegH,0);
    Write_MFRC522(TModeReg,0x8D);
    Write_MFRC522(TPrescalerReg,0x3E);
    Write_MFRC522(TxAutoReg,0x40);
    return MI_OK;
}

/////////////////////////////////////////////////////////////////////
//功    能：置RC522寄存器位
//参数说明：reg[IN]:寄存器地址
//          mask[IN]:置位值
/////////////////////////////////////////////////////////////////////
void SetBitMask(unsigned char reg,unsigned char mask)  
{
    char tmp = 0x0;
    tmp = Read_MFRC522(reg);
    Write_MFRC522(reg,tmp | mask);  // set bit mask
}



/////////////////////////////////////////////////////////////////////
//功    能：清RC522寄存器位
//参数说明：reg[IN]:寄存器地址
//          mask[IN]:清位值
/////////////////////////////////////////////////////////////////////
void ClearBitMask(unsigned char reg,unsigned char mask)  
{
    char tmp = 0x0;
    tmp = Read_MFRC522(reg);
    Write_MFRC522(reg, tmp & ~mask);  // clear bit mask
} 


/////////////////////////////////////////////////////////////////////
//功    能：通过RC522和ISO14443卡通讯
//参数说明：Command[IN]:RC522命令字
//          pInData[IN]:通过RC522发送到卡片的数据
//          InLenByte[IN]:发送数据的字节长度
//          pOutData[OUT]:接收到的卡片返回数据
//          *pOutLenBit[OUT]:返回数据的位长度
/////////////////////////////////////////////////////////////////////
char MFRC522_ToCard(unsigned char Command, 
                 unsigned char *pInData, 
                 unsigned char InLenByte,
                 unsigned char *pOutData, 
                 unsigned int  *pOutLenBit)
{
    char status = MI_ERR;
    unsigned char irqEn   = 0x00;
    unsigned char waitFor = 0x00;
    unsigned char lastBits;
    unsigned char n;
    unsigned int i;
    switch (Command)
    {
       case PCD_AUTHENT:
          irqEn   = 0x12;
          waitFor = 0x10;
          break;
       case PCD_TRANSCEIVE:
          irqEn   = 0x77;
          waitFor = 0x30;
          break;
       default:
         break;
    }
   
    Write_MFRC522(ComIEnReg,irqEn|0x80);	//PCD_TRANSCEIVE模式 无命令改变
    ClearBitMask(ComIrqReg,0x80);			//IRQ开漏输出
    Write_MFRC522(CommandReg,PCD_IDLE);  //取消当前命令
    SetBitMask(FIFOLevelReg,0x80);		//清除FIFO Flash 内ErrReg  BufferOvfl标志
    
    for (i=0; i<InLenByte; i++)
    {   
		Write_MFRC522(FIFODataReg, pInData[i]);    //把数据存到FIFO
	}
    Write_MFRC522(CommandReg, Command);   //发送FIFO内容
   
    
    if (Command == PCD_TRANSCEIVE)
    {    
		SetBitMask(BitFramingReg,0x80);  //立即停止定时器
	}
    
	n = Read_MFRC522(ComIrqReg);
    i = 1500;//根据时钟频率调整，操作M1卡最大等待时间25ms
    do 
    {
        n = Read_MFRC522(ComIrqReg);
        
        i--;
			
    }
    while ((i!=0) && !(n&0x01) && !(n&waitFor));
    ClearBitMask(BitFramingReg,0x80);
	      
    if (i!=0)
    {    
        if(!(Read_MFRC522(ErrorReg)&0x1B))
        {
            status = MI_OK;
            if (n & irqEn & 0x01)
            {   status = MI_NOTAGERR;   }
            if (Command == PCD_TRANSCEIVE)
            {
            n = Read_MFRC522(FIFOLevelReg);
            lastBits = Read_MFRC522(ControlReg) & 0x07;
            if (lastBits)
            {   *pOutLenBit = (n-1)*8 + lastBits;   }
            else
            {   *pOutLenBit = n*8;   }
            if (n == 0)
            {   n = 1;    }
            if (n > MAXRLEN)
            {   n = MAXRLEN;   }
            for (i=0; i<n; i++)
            {   pOutData[i] = Read_MFRC522(FIFODataReg);    }
        }
        }
        else
        {   
            status = MI_ERR;   
        }
        
   }
   

   SetBitMask(ControlReg,0x80);           // stop timer now
   Write_MFRC522(CommandReg,PCD_IDLE); 
   return status;
}




//开启天线  
//每次启动或关闭天线发射之间应至少有1ms的间隔
void MFRC522_AntennaOn(void)
{
    unsigned char i;
    i = Read_MFRC522(TxControlReg);
    if (!(i & 0x03))
    {
        SetBitMask(TxControlReg, 0x03);
    }
}


//关闭天线
void MFRC522_AntennaOff(void)
{
    ClearBitMask(TxControlReg, 0x03);
}


//功能描述?用MF522计算CRC
//输入参数?pIndata--要读数CRC的数据?len--数据长度?pOutData--计算的CRC结果
void CalulateCRC(unsigned char *pIndata,unsigned char len,unsigned char *pOutData)
{
    unsigned char i,n;
    ClearBitMask(DivIrqReg,0x04);
    Write_MFRC522(CommandReg,PCD_IDLE);
    SetBitMask(FIFOLevelReg,0x80);
    for (i=0; i<len; i++)
    {   Write_MFRC522(FIFODataReg, *(pIndata+i));   }
    Write_MFRC522(CommandReg, PCD_CALCCRC);
    i = 0xFF;
    do 
    {
        n = Read_MFRC522(DivIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x04));
    pOutData[0] = Read_MFRC522(CRCResultRegL);
    pOutData[1] = Read_MFRC522(CRCResultRegM);
}


/////////////////////////////////////////////////////////////////////
//功    能：命令卡片进入休眠状态
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char MFRC522_Halt(void)
{
    
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 
	char status;
    ucComMF522Buf[0] = PICC_HALT;
    ucComMF522Buf[1] = 0;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = MFRC522_ToCard(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    return MI_OK;
}




/////////////////////////////////////////////////////////////////////
//功    能：寻卡
//参数说明:        req_code[IN]:寻卡方式
//                0x52 = 寻感应区内所有符合14443A标准的卡
//                0x26 = 寻未进入休眠状态的卡
//                pTagType[OUT]：卡片类型代码
//                0x4400 = Mifare_UltraLight
//                0x0400 = Mifare_One(S50)
//                0x0200 = Mifare_One(S70)
//                0x0800 = Mifare_Pro(X)
//                0x4403 = Mifare_DESFire
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char MFRC522_Request(unsigned char req_code,unsigned char *pTagType)
{
   char status;  
   unsigned int  unLen;
   unsigned char ucComMF522Buf[MAXRLEN]; 

   ClearBitMask(Status2Reg,0x08);
   Write_MFRC522(BitFramingReg,0x07);
   SetBitMask(TxControlReg,0x03);
 
   ucComMF522Buf[0] = req_code;

   status = MFRC522_ToCard(PCD_TRANSCEIVE,ucComMF522Buf,1,ucComMF522Buf,&unLen);
   
   if ((status == MI_OK) && (unLen == 0x10))
   {    
       *pTagType     = ucComMF522Buf[0];
       *(pTagType+1) = ucComMF522Buf[1];
   }
   else
   {   status = MI_ERR;  

    }
   
   return status;
}



//功    能：防冲突检测?读取选中卡片的卡序列号
//参数说明: pSnr[OUT]:卡片序列号，4字节
//返    回: 成功返回MI_OK 
char MFRC522_Anticoll(unsigned char *pSnr)
{
    char status;
    unsigned char i,snr_check=0;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 
    

    ClearBitMask(Status2Reg,0x08);
    Write_MFRC522(BitFramingReg,0x00);
    ClearBitMask(CollReg,0x80);
 
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x20;

    status = MFRC522_ToCard(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&unLen);

    if (status == MI_OK)
    {
        for (i=0; i<4; i++)
        {   
        *(pSnr+i)  = ucComMF522Buf[i];
        snr_check ^= ucComMF522Buf[i];
        }
        if (snr_check != ucComMF522Buf[i])
        {   status = MI_ERR;    }
    }
    
    SetBitMask(CollReg,0x80);
    return status;
}


/////////////////////////////////////////////////////////////////////
//功    能：选定卡片
//参数说明: pSnr[IN]:卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char MFRC522_SelectTag(unsigned char *pSnr)
{
    char status;
    unsigned char i;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x70;
    ucComMF522Buf[6] = 0;
    for (i=0; i<4; i++)
    {
    	ucComMF522Buf[i+2] = *(pSnr+i);
    	ucComMF522Buf[6]  ^= *(pSnr+i);
    }
    CalulateCRC(ucComMF522Buf,7,&ucComMF522Buf[7]);
  
    ClearBitMask(Status2Reg,0x08);

    status = MFRC522_ToCard(PCD_TRANSCEIVE,ucComMF522Buf,9,ucComMF522Buf,&unLen);
    
    if ((status == MI_OK) && (unLen == 0x18))
    {   status = MI_OK;  }
    else
    {   status = MI_ERR;    }

    return status;
}



/////////////////////////////////////////////////////////////////////
// 功    能：验证卡片密码
// 参数说明: 
//    auth_mode[IN]：密码验证模式
//                 0x60 = 验证A密钥
//                 0x61 = 验证B密钥 
//    addr[IN]：块地址
//    pKey[IN]：密码
//    pSnr[IN]：卡片序列号，4字节
// 返    回: 成功返回 MI_OK
/////////////////////////////////////////////////////////////////////
char MFRC522_AuthState(unsigned char auth_mode, unsigned char addr, unsigned char *pKey, unsigned char *pSnr)
{
    char status;
    unsigned int unLen = 0; // 确保初始化
    unsigned char i, ucComMF522Buf[MAXRLEN]; // 通信缓冲区

    ucComMF522Buf[0] = auth_mode; // 验证模式
    ucComMF522Buf[1] = addr;      // 块地址

    // 使用循环赋值密码到缓冲区
    for (i = 0; i < 6; i++) {
        ucComMF522Buf[i + 2] = *(pKey + i);
    }
    // 使用循环赋值序列号（4字节）到缓冲区
    for (i = 0; i < 4; i++) {
        ucComMF522Buf[i + 8] = *(pSnr + i);
    }

    // 以下两行可以替代上述循环赋值
    // memcpy(&ucComMF522Buf[2], pKey, 6); // 复制密码
    // memcpy(&ucComMF522Buf[8], pSnr, 4); // 复制序列号

    // 发送命令
    status = MFRC522_ToCard(PCD_AUTHENT, ucComMF522Buf, 12, ucComMF522Buf, &unLen);

    // 检查状态寄存器，确认认证是否成功
    if ((status != MI_OK) || (!(Read_MFRC522(Status2Reg) & 0x08))) {
        status = MI_ERR;
    }

    return status;
}



/////////////////////////////////////////////////////////////////////
//功    能：读取M1卡一块数据
//参数说明: addr[IN]：块地址
//          pData[OUT]：读出的数据，16字节
//返    回: 成功返回MI_OK
///////////////////////////////////////////////////////////////////// 
char MFRC522_Read(unsigned char addr,unsigned char *pData)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_READ;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
   
    status = MFRC522_ToCard(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
    if ((status == MI_OK) && (unLen == 0x90))
 //   {   memcpy(pData, ucComMF522Buf, 16);   }
    {
        for (i=0; i<16; i++)
        {    *(pData+i) = ucComMF522Buf[i];   }
    }
    else
    {   status = MI_ERR;   }
    
    return status;
}


/////////////////////////////////////////////////////////////////////
// 函数名称: MFRC522_Write
// 功能描述: 向M1卡的指定块写入16字节数据，只负责写，不负责验证扇区密码逻辑
// 输入参数: addr  - 要写入的块地址
//          pData - 指向要写入的16字节数据的指针
// 返回值  : MI_OK  - 写入成功
//          MI_ERR - 写入失败
/////////////////////////////////////////////////////////////////////
char MFRC522_Write(unsigned char addr, unsigned char *pData)
{
    char status;                     // 用于存储函数执行状态
    unsigned int unLen;              // 用于存储接收到的数据长度
    unsigned char i;                 // 循环计数器
    unsigned char ucComMF522Buf[MAXRLEN];  // 通信缓冲区
    
    // 组装写命令帧
    ucComMF522Buf[0] = PICC_WRITE;  // 写命令
    ucComMF522Buf[1] = addr;        // 块地址
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);  // 计算CRC并添加到命令帧末尾
 
    // 发送写命令并验证卡片应答
    status = MFRC522_ToCard(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    // 检查应答状态
    // 1. status必须为MI_OK
    // 2. 接收到的数据长度必须为4
    // 3. 应答的低4位必须为0x0A
    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   
        status = MI_ERR;   
    }
        
    if (status == MI_OK)
    {
        //memcpy(ucComMF522Buf, pData, 16);
        // 准备写入数据
        // 将16字节数据复制到发送缓冲区
        for (i=0; i<16; i++)
        {    
            ucComMF522Buf[i] = *(pData+i);   
        }
        CalulateCRC(ucComMF522Buf,16,&ucComMF522Buf[16]);  // 计算数据的CRC值

        // 发送数据块并验证卡片应答
        status = MFRC522_ToCard(PCD_TRANSCEIVE,ucComMF522Buf,18,ucComMF522Buf,&unLen);
        // 再次检查应答状态
        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {   
            status = MI_ERR;   
        }
    }
    
    return status;
}

/***********************************************
*函数名    ：WriteCardData
*函数功能  ：往卡片的某个块地址写数据（有验证密码逻辑）
*函数参数  ：u8 addr,u8 *data,u8 *pSnr    
*函数返回值：u8
*函数描述  ：块的绝对地址0~63  注意不写到控制块中
            要写的数据  一定是16个字节
            pSnr  是卡片序列号，4字节
***********************************************/
u8 WriteCardData(u8 addr,u8 *data,u8 *pSnr){
    u8 pTagType[2] = {0};     //存卡的型号数据
    /*寻卡*/
	if(MFRC522_Request(PICC_REQALL,pTagType)  != MI_OK) 
	{
        MFRC522_Reset();
        MFRC522_AntennaOff(); 
        MFRC522_AntennaOn();
		return 1;
	}

	/*防冲撞*/
    //把卡的序列号保存在pSnr中传递出去
	if(MFRC522_Anticoll(pSnr)  != MI_OK)
	{
		return 2;
	}
	
	/*选卡*/
	if(MFRC522_SelectTag(pSnr)  != MI_OK)
	{
		return 3;
	}

    //嘟一声
    voice(Di);
    /*密码验证*/
    //数据块与控制块地址关系 = (addr / 4) * 4 + 3
	if(MFRC522_AuthState(PICC_AUTHENT1A,addr/4*4+3,picc_passward,pSnr) != MI_OK)//验证A密码
	{
		printf("密码A对比失败!\r\n");
		MFRC522_Halt();
		return 4;
	}
    /*写数据*/
	if(MFRC522_Write(addr,data) != MI_OK)
	{
		printf("数据写入失败!\r\n");
		MFRC522_Halt();
		return 5;	
	}
    /*休眠*/
	MFRC522_Halt();
	printf("数据写入成功\r\n");

    return MI_OK;
}


/**
 * @brief 就是把找寻到的卡片序列号传递出去
 * 
 * @param pSnr 
 */
u8 MatchCard(u8 *pSnr){
    u8 pTagType[2] = {0};     //存卡的型号数据

    /*寻卡*/
	if(MFRC522_Request(PICC_REQALL,pTagType)  != MI_OK) 
	{
        MFRC522_Reset();
        MFRC522_AntennaOff(); 
        MFRC522_AntennaOn();
		return 1;
	}

	/*防冲撞*/
    //把卡的序列号保存在pSnr中，传递出去
	if(MFRC522_Anticoll(pSnr)  != MI_OK)
	{
		return 2;
	}

    return MI_OK;


}




//测试函数
unsigned char Test_MFRC522()
{
    unsigned char status;
    unsigned int temp, i;
    unsigned char buf[20];

    status = MFRC522_Request(PICC_REQALL, buf); // 寻卡
    if (status != MI_OK)
    {    
        MFRC522_Reset();
        MFRC522_AntennaOff(); 
        MFRC522_AntennaOn(); 
        return 0;
    }

    printf("卡的类型:");
    for(i = 0; i < 2; i++)
    {
        temp = buf[i];
        printf("%X", temp);
    }
    
    status = MFRC522_Anticoll(buf); // 防冲突
    if (status != MI_OK)
    {    
        return 0;    
    }
    
    printf("卡序列号：");
    for(i = 0; i < 4; i++)
    {
        temp = buf[i];
        printf("%X", temp);
    }
    
    printf("\r\n");
    return 1;
}

//修改密码的测试函数
//修改前后密码认证按需修改一下，有一个小bug,显示密码A时不能正确显示，其他正常，数据写入也正常
void Test_ChangeSector15Password() {
    unsigned char status;
    //额外的一个字节用于可能的校验码或结束标志，操作按四个字节操作
    unsigned char snr[5]; // 卡片UID
    unsigned char trailer_data[16]; // 扇区尾块数据缓存
    unsigned char pass_A[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // 默认密码A
    unsigned char pass_Apro[6] = {0x12, 0x12, 0x12, 0x12, 0x12, 0x12};//复位用于测试的密码A，记得修改
    //使用const确保数据不被意外修改
    const unsigned char pass_new[16] = { // 新的扇区尾块数据
        0x12, 0x12, 0x12, 0x12, 0x12, 0x12, // 新密码A
        0xFF, 0x07, 0x80, 0x69,             // 访问控制字节（保持不变）
        0x13, 0x13, 0x13, 0x13, 0x13, 0x13  // 新密码B
    };
    //使用const确保数据不被意外修改
    const unsigned char pass_old[16] = { // 新的扇区尾块数据
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // 默认密码A
        0xFF, 0x07, 0x80, 0x69,             // 访问控制字节（保持不变）
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF  // 默认密码B
    };
    //只修改一次，防止在while（1）反复修改
    static unsigned char flag = 1; // 静态变量，保证多次调用时状态保持一致


    if(flag == 1){
        printf("=== 开始更改扇区15密码 ===\r\n");

        // 1. 寻卡
        unsigned char cardType[2];
        status = MFRC522_Request(0x52, cardType); // 0x52 = 寻感应区内所有的卡
        if (status != MI_OK) {
            printf("寻卡失败！\r\n");
            MFRC522_Reset();
            MFRC522_AntennaOff(); 
            MFRC522_AntennaOn(); 
            return;
        }
        printf("卡片类型：%02X%02X\n", cardType[0], cardType[1]);

        // 2. 防冲突，获取卡片UID
        status = MFRC522_Anticoll(snr);
        if (status != MI_OK) {
            printf("获取卡片UID失败!\r\n");
            return;
        }
        printf("卡片UID:%02X %02X %02X %02X\r\n", snr[0], snr[1], snr[2], snr[3]);

        // 3. 选中卡片
        status = MFRC522_SelectTag(snr);
        if (status != MI_OK) {
            printf("选卡失败！\r\n");
            return;
        }
        printf("卡片已选中。\r\n");

        // 4. 验证密码A,扇区15的块号（每个扇区4块，最后一块是扇区尾块）
        status = MFRC522_AuthState(0x60, 63, pass_A, snr); // 0x60 = 验证密码A
        if (status != MI_OK) {
            printf("密码A认证失败!\r\n");
            return;
        }
        printf("密码A认证成功。\r\n");

        // 5. 读取扇区15的尾块数据(修改确实成功了，密码也能认证，但是获取不高密码A，其他字节数据可以)
        status = MFRC522_Read(63, trailer_data);
        if (status != MI_OK) {
            printf("读取扇区15尾块数据失败!\r\n");
            return;
        }
        printf("当前扇区15尾块数据:");
        for (int i = 0; i < 16; i++) {
            printf("%02X ", trailer_data[i]);
        }
        printf("\r\n");

        // 6. 更改密码A和密码B
        memcpy(trailer_data, pass_old, 16);
        status = MFRC522_Write(63, trailer_data);
        if (status != MI_OK) {
            printf("写入新密码失败！\r\n");
            return;
        }
        printf("新密码写入成功。\r\n");

        // 7. 测试新密码是否生效
        unsigned char pass_A_new[6] = {0x12, 0x12, 0x12, 0x12, 0x12, 0x12};
        unsigned char pass_A_old[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
        status = MFRC522_AuthState(0x60,63, pass_A_old, snr); // 用新密码A验证
        if (status != MI_OK) {
            printf("新密码认证失败！\r\n");
            return;
        }
        printf("新密码认证成功,扇区15密码修改完成!\r\n");

        // 8. 停止对卡片操作
        MFRC522_Halt();
        printf("卡片操作已停止。\r\n");
        printf("=== 扇区15密码修改流程完成 ===\r\n");

        flag = 0;
    }else {
        printf("密码已经修改过一次！\r\n");
    }
}