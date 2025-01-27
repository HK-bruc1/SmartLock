#include "main.h"


/*****************���絥Ƭ�����******************
											STM32
 * �ļ�			:	MFRC522��Ƶģ��c�ļ�                   
 * �汾			: V1.0
 * ����			: 2024.9.2
 * MCU			:	STM32F103C8T6
 * �ӿ�			:	������							
 * BILIBILI	:	���絥Ƭ�����
 * CSDN			:	���絥Ƭ�����
 * ����			:	����

**********************BEGIN***********************/


/*********************************************************
*������    ��MFRC522_Init
*��������  ��RFID����SPI��GPIO��ʼ�������Լ�����Ӳ����GPIO�ڳ�ʼ��
*��������  ����
*��������ֵ����
*��������  ��
*              PA15--CS--�������
*              PC10--SCL--�������
*              PC11--MISO--��������
*              PC12--MOSI--�������
*              PD2--RST--�������
*********************************************************/
void MFRC522_Init(void)
{
    //GPIO��ʼ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC |RCC_AHB1Periph_GPIOD, ENABLE);//1.��ʱ��
	
	//PC10--SCL--�������,PC12--MOSI--�������
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	//PA15--CS--�������
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	//PD2--RST--�������
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOD, &GPIO_InitStruct);
	
	//PC11--MISO--��������
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	//Ƭѡ���ߣ���λ������
	GPIO_SetBits(GPIOD, GPIO_Pin_2);
	GPIO_SetBits(GPIOA,GPIO_Pin_15);
}


//����һ���ֽڣ�дRC522�Ĵ���ʱ����ã�
//ʱ�Ӽ��� 1 ��
//ʱ����λ 1 ��  
u8 SPITransferByte(u8 data){
    u8 i;

    MFRC522_SCK_H;//��������

    for(i=0;i<8;i++){
        //����һλ����
        MFRC522_SCK_L;
        if(data & 0x80)
            MFRC522_MOSI_H;
        else
            MFRC522_MOSI_L;
        data <<= 1;
        
        //����һλ����
        MFRC522_SCK_H;
        if(MFRC522_MISO_READ)
            data |= 0x01;
    }
    return data;
}

	

/////////////////////////////////////////////////////////////////////
//��������?��MFRC522��ĳһ�Ĵ�����һ���ֽ�����
//����˵����Address[IN]:�Ĵ�����ַ
//��    �أ�������ֵ
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
//��������?��MFRC522��ĳһ�Ĵ���дһ���ֽ�����
//����˵����Address[IN]:�Ĵ�����ַ
//          value[IN]:д���ֵ
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
//��    �ܣ���λRC522
//��    ��: �ɹ�����MI_OK
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

    Write_MFRC522(ModeReg,0x3D);            //��Mifare��ͨѶ��CRC��ʼֵ0x6363
    Write_MFRC522(TReloadRegL,30);           
    Write_MFRC522(TReloadRegH,0);
    Write_MFRC522(TModeReg,0x8D);
    Write_MFRC522(TPrescalerReg,0x3E);
    Write_MFRC522(TxAutoReg,0x40);
    return MI_OK;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ���RC522�Ĵ���λ
//����˵����reg[IN]:�Ĵ�����ַ
//          mask[IN]:��λֵ
/////////////////////////////////////////////////////////////////////
void SetBitMask(unsigned char reg,unsigned char mask)  
{
    char tmp = 0x0;
    tmp = Read_MFRC522(reg);
    Write_MFRC522(reg,tmp | mask);  // set bit mask
}



/////////////////////////////////////////////////////////////////////
//��    �ܣ���RC522�Ĵ���λ
//����˵����reg[IN]:�Ĵ�����ַ
//          mask[IN]:��λֵ
/////////////////////////////////////////////////////////////////////
void ClearBitMask(unsigned char reg,unsigned char mask)  
{
    char tmp = 0x0;
    tmp = Read_MFRC522(reg);
    Write_MFRC522(reg, tmp & ~mask);  // clear bit mask
} 


/////////////////////////////////////////////////////////////////////
//��    �ܣ�ͨ��RC522��ISO14443��ͨѶ
//����˵����Command[IN]:RC522������
//          pInData[IN]:ͨ��RC522���͵���Ƭ������
//          InLenByte[IN]:�������ݵ��ֽڳ���
//          pOutData[OUT]:���յ��Ŀ�Ƭ��������
//          *pOutLenBit[OUT]:�������ݵ�λ����
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
   
    Write_MFRC522(ComIEnReg,irqEn|0x80);	//PCD_TRANSCEIVEģʽ ������ı�
    ClearBitMask(ComIrqReg,0x80);			//IRQ��©���
    Write_MFRC522(CommandReg,PCD_IDLE);  //ȡ����ǰ����
    SetBitMask(FIFOLevelReg,0x80);		//���FIFO Flash ��ErrReg  BufferOvfl��־
    
    for (i=0; i<InLenByte; i++)
    {   
		Write_MFRC522(FIFODataReg, pInData[i]);    //�����ݴ浽FIFO
	}
    Write_MFRC522(CommandReg, Command);   //����FIFO����
   
    
    if (Command == PCD_TRANSCEIVE)
    {    
		SetBitMask(BitFramingReg,0x80);  //����ֹͣ��ʱ��
	}
    
	n = Read_MFRC522(ComIrqReg);
    i = 1500;//����ʱ��Ƶ�ʵ���������M1�����ȴ�ʱ��25ms
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




//��������  
//ÿ��������ر����߷���֮��Ӧ������1ms�ļ��
void MFRC522_AntennaOn(void)
{
    unsigned char i;
    i = Read_MFRC522(TxControlReg);
    if (!(i & 0x03))
    {
        SetBitMask(TxControlReg, 0x03);
    }
}


//�ر�����
void MFRC522_AntennaOff(void)
{
    ClearBitMask(TxControlReg, 0x03);
}


//��������?��MF522����CRC
//�������?pIndata--Ҫ����CRC������?len--���ݳ���?pOutData--�����CRC���
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
//��    �ܣ����Ƭ��������״̬
//��    ��: �ɹ�����MI_OK
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
//��    �ܣ�Ѱ��
//����˵��:        req_code[IN]:Ѱ����ʽ
//                0x52 = Ѱ��Ӧ�������з���14443A��׼�Ŀ�
//                0x26 = Ѱδ��������״̬�Ŀ�
//                pTagType[OUT]����Ƭ���ʹ���
//                0x4400 = Mifare_UltraLight
//                0x0400 = Mifare_One(S50)
//                0x0200 = Mifare_One(S70)
//                0x0800 = Mifare_Pro(X)
//                0x4403 = Mifare_DESFire
//��    ��: �ɹ�����MI_OK
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



//��    �ܣ�����ͻ���?��ȡѡ�п�Ƭ�Ŀ����к�
//����˵��: pSnr[OUT]:��Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK 
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
//��    �ܣ�ѡ����Ƭ
//����˵��: pSnr[IN]:��Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
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
// ��    �ܣ���֤��Ƭ����
// ����˵��: 
//    auth_mode[IN]��������֤ģʽ
//                 0x60 = ��֤A��Կ
//                 0x61 = ��֤B��Կ 
//    addr[IN]�����ַ
//    pKey[IN]������
//    pSnr[IN]����Ƭ���кţ�4�ֽ�
// ��    ��: �ɹ����� MI_OK
/////////////////////////////////////////////////////////////////////
char MFRC522_AuthState(unsigned char auth_mode, unsigned char addr, unsigned char *pKey, unsigned char *pSnr)
{
    char status;
    unsigned int unLen = 0; // ȷ����ʼ��
    unsigned char i, ucComMF522Buf[MAXRLEN]; // ͨ�Ż�����

    ucComMF522Buf[0] = auth_mode; // ��֤ģʽ
    ucComMF522Buf[1] = addr;      // ���ַ

    // ʹ��ѭ����ֵ���뵽������
    for (i = 0; i < 6; i++) {
        ucComMF522Buf[i + 2] = *(pKey + i);
    }
    // ʹ��ѭ����ֵ���кţ�4�ֽڣ���������
    for (i = 0; i < 4; i++) {
        ucComMF522Buf[i + 8] = *(pSnr + i);
    }

    // �������п����������ѭ����ֵ
    // memcpy(&ucComMF522Buf[2], pKey, 6); // ��������
    // memcpy(&ucComMF522Buf[8], pSnr, 4); // �������к�

    // ��������
    status = MFRC522_ToCard(PCD_AUTHENT, ucComMF522Buf, 12, ucComMF522Buf, &unLen);

    // ���״̬�Ĵ�����ȷ����֤�Ƿ�ɹ�
    if ((status != MI_OK) || (!(Read_MFRC522(Status2Reg) & 0x08))) {
        status = MI_ERR;
    }

    return status;
}



/////////////////////////////////////////////////////////////////////
//��    �ܣ���ȡM1��һ������
//����˵��: addr[IN]�����ַ
//          pData[OUT]�����������ݣ�16�ֽ�
//��    ��: �ɹ�����MI_OK
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
// ��������: MFRC522_Write
// ��������: ��M1����ָ����д��16�ֽ����ݣ�ֻ����д����������֤���������߼�
// �������: addr  - Ҫд��Ŀ��ַ
//          pData - ָ��Ҫд���16�ֽ����ݵ�ָ��
// ����ֵ  : MI_OK  - д��ɹ�
//          MI_ERR - д��ʧ��
/////////////////////////////////////////////////////////////////////
char MFRC522_Write(unsigned char addr, unsigned char *pData)
{
    char status;                     // ���ڴ洢����ִ��״̬
    unsigned int unLen;              // ���ڴ洢���յ������ݳ���
    unsigned char i;                 // ѭ��������
    unsigned char ucComMF522Buf[MAXRLEN];  // ͨ�Ż�����
    
    // ��װд����֡
    ucComMF522Buf[0] = PICC_WRITE;  // д����
    ucComMF522Buf[1] = addr;        // ���ַ
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);  // ����CRC����ӵ�����֡ĩβ
 
    // ����д�����֤��ƬӦ��
    status = MFRC522_ToCard(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    // ���Ӧ��״̬
    // 1. status����ΪMI_OK
    // 2. ���յ������ݳ��ȱ���Ϊ4
    // 3. Ӧ��ĵ�4λ����Ϊ0x0A
    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   
        status = MI_ERR;   
    }
        
    if (status == MI_OK)
    {
        //memcpy(ucComMF522Buf, pData, 16);
        // ׼��д������
        // ��16�ֽ����ݸ��Ƶ����ͻ�����
        for (i=0; i<16; i++)
        {    
            ucComMF522Buf[i] = *(pData+i);   
        }
        CalulateCRC(ucComMF522Buf,16,&ucComMF522Buf[16]);  // �������ݵ�CRCֵ

        // �������ݿ鲢��֤��ƬӦ��
        status = MFRC522_ToCard(PCD_TRANSCEIVE,ucComMF522Buf,18,ucComMF522Buf,&unLen);
        // �ٴμ��Ӧ��״̬
        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {   
            status = MI_ERR;   
        }
    }
    
    return status;
}

/***********************************************
*������    ��WriteCardData
*��������  ������Ƭ��ĳ�����ַд���ݣ�����֤�����߼���
*��������  ��u8 addr,u8 *data,u8 *pSnr    
*��������ֵ��u8
*��������  ����ľ��Ե�ַ0~63  ע�ⲻд�����ƿ���
            Ҫд������  һ����16���ֽ�
            pSnr  �ǿ�Ƭ���кţ�4�ֽ�
***********************************************/
u8 WriteCardData(u8 addr,u8 *data,u8 *pSnr){
    u8 pTagType[2] = {0};     //�濨���ͺ�����
    /*Ѱ��*/
	if(MFRC522_Request(PICC_REQALL,pTagType)  != MI_OK) 
	{
        MFRC522_Reset();
        MFRC522_AntennaOff(); 
        MFRC522_AntennaOn();
		return 1;
	}

	/*����ײ*/
    //�ѿ������кű�����pSnr�д��ݳ�ȥ
	if(MFRC522_Anticoll(pSnr)  != MI_OK)
	{
		return 2;
	}
	
	/*ѡ��*/
	if(MFRC522_SelectTag(pSnr)  != MI_OK)
	{
		return 3;
	}

    //�һ��
    voice(Di);
    /*������֤*/
    //���ݿ�����ƿ��ַ��ϵ = (addr / 4) * 4 + 3
	if(MFRC522_AuthState(PICC_AUTHENT1A,addr/4*4+3,picc_passward,pSnr) != MI_OK)//��֤A����
	{
		printf("����A�Ա�ʧ��!\r\n");
		MFRC522_Halt();
		return 4;
	}
    /*д����*/
	if(MFRC522_Write(addr,data) != MI_OK)
	{
		printf("����д��ʧ��!\r\n");
		MFRC522_Halt();
		return 5;	
	}
    /*����*/
	MFRC522_Halt();
	printf("����д��ɹ�\r\n");

    return MI_OK;
}


/**
 * @brief ���ǰ���Ѱ���Ŀ�Ƭ���кŴ��ݳ�ȥ
 * 
 * @param pSnr 
 */
u8 MatchCard(u8 *pSnr){
    u8 pTagType[2] = {0};     //�濨���ͺ�����

    /*Ѱ��*/
	if(MFRC522_Request(PICC_REQALL,pTagType)  != MI_OK) 
	{
        MFRC522_Reset();
        MFRC522_AntennaOff(); 
        MFRC522_AntennaOn();
		return 1;
	}

	/*����ײ*/
    //�ѿ������кű�����pSnr�У����ݳ�ȥ
	if(MFRC522_Anticoll(pSnr)  != MI_OK)
	{
		return 2;
	}

    return MI_OK;


}




//���Ժ���
unsigned char Test_MFRC522()
{
    unsigned char status;
    unsigned int temp, i;
    unsigned char buf[20];

    status = MFRC522_Request(PICC_REQALL, buf); // Ѱ��
    if (status != MI_OK)
    {    
        MFRC522_Reset();
        MFRC522_AntennaOff(); 
        MFRC522_AntennaOn(); 
        return 0;
    }

    printf("��������:");
    for(i = 0; i < 2; i++)
    {
        temp = buf[i];
        printf("%X", temp);
    }
    
    status = MFRC522_Anticoll(buf); // ����ͻ
    if (status != MI_OK)
    {    
        return 0;    
    }
    
    printf("�����кţ�");
    for(i = 0; i < 4; i++)
    {
        temp = buf[i];
        printf("%X", temp);
    }
    
    printf("\r\n");
    return 1;
}

//�޸�����Ĳ��Ժ���
//�޸�ǰ��������֤�����޸�һ�£���һ��Сbug,��ʾ����Aʱ������ȷ��ʾ����������������д��Ҳ����
void Test_ChangeSector15Password() {
    unsigned char status;
    //�����һ���ֽ����ڿ��ܵ�У����������־���������ĸ��ֽڲ���
    unsigned char snr[5]; // ��ƬUID
    unsigned char trailer_data[16]; // ����β�����ݻ���
    unsigned char pass_A[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // Ĭ������A
    unsigned char pass_Apro[6] = {0x12, 0x12, 0x12, 0x12, 0x12, 0x12};//��λ���ڲ��Ե�����A���ǵ��޸�
    //ʹ��constȷ�����ݲ��������޸�
    const unsigned char pass_new[16] = { // �µ�����β������
        0x12, 0x12, 0x12, 0x12, 0x12, 0x12, // ������A
        0xFF, 0x07, 0x80, 0x69,             // ���ʿ����ֽڣ����ֲ��䣩
        0x13, 0x13, 0x13, 0x13, 0x13, 0x13  // ������B
    };
    //ʹ��constȷ�����ݲ��������޸�
    const unsigned char pass_old[16] = { // �µ�����β������
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Ĭ������A
        0xFF, 0x07, 0x80, 0x69,             // ���ʿ����ֽڣ����ֲ��䣩
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF  // Ĭ������B
    };
    //ֻ�޸�һ�Σ���ֹ��while��1�������޸�
    static unsigned char flag = 1; // ��̬��������֤��ε���ʱ״̬����һ��


    if(flag == 1){
        printf("=== ��ʼ��������15���� ===\r\n");

        // 1. Ѱ��
        unsigned char cardType[2];
        status = MFRC522_Request(0x52, cardType); // 0x52 = Ѱ��Ӧ�������еĿ�
        if (status != MI_OK) {
            printf("Ѱ��ʧ�ܣ�\r\n");
            MFRC522_Reset();
            MFRC522_AntennaOff(); 
            MFRC522_AntennaOn(); 
            return;
        }
        printf("��Ƭ���ͣ�%02X%02X\n", cardType[0], cardType[1]);

        // 2. ����ͻ����ȡ��ƬUID
        status = MFRC522_Anticoll(snr);
        if (status != MI_OK) {
            printf("��ȡ��ƬUIDʧ��!\r\n");
            return;
        }
        printf("��ƬUID:%02X %02X %02X %02X\r\n", snr[0], snr[1], snr[2], snr[3]);

        // 3. ѡ�п�Ƭ
        status = MFRC522_SelectTag(snr);
        if (status != MI_OK) {
            printf("ѡ��ʧ�ܣ�\r\n");
            return;
        }
        printf("��Ƭ��ѡ�С�\r\n");

        // 4. ��֤����A,����15�Ŀ�ţ�ÿ������4�飬���һ��������β�飩
        status = MFRC522_AuthState(0x60, 63, pass_A, snr); // 0x60 = ��֤����A
        if (status != MI_OK) {
            printf("����A��֤ʧ��!\r\n");
            return;
        }
        printf("����A��֤�ɹ���\r\n");

        // 5. ��ȡ����15��β������(�޸�ȷʵ�ɹ��ˣ�����Ҳ����֤�����ǻ�ȡ��������A�������ֽ����ݿ���)
        status = MFRC522_Read(63, trailer_data);
        if (status != MI_OK) {
            printf("��ȡ����15β������ʧ��!\r\n");
            return;
        }
        printf("��ǰ����15β������:");
        for (int i = 0; i < 16; i++) {
            printf("%02X ", trailer_data[i]);
        }
        printf("\r\n");

        // 6. ��������A������B
        memcpy(trailer_data, pass_old, 16);
        status = MFRC522_Write(63, trailer_data);
        if (status != MI_OK) {
            printf("д��������ʧ�ܣ�\r\n");
            return;
        }
        printf("������д��ɹ���\r\n");

        // 7. �����������Ƿ���Ч
        unsigned char pass_A_new[6] = {0x12, 0x12, 0x12, 0x12, 0x12, 0x12};
        unsigned char pass_A_old[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
        status = MFRC522_AuthState(0x60,63, pass_A_old, snr); // ��������A��֤
        if (status != MI_OK) {
            printf("��������֤ʧ�ܣ�\r\n");
            return;
        }
        printf("��������֤�ɹ�,����15�����޸����!\r\n");

        // 8. ֹͣ�Կ�Ƭ����
        MFRC522_Halt();
        printf("��Ƭ������ֹͣ��\r\n");
        printf("=== ����15�����޸�������� ===\r\n");

        flag = 0;
    }else {
        printf("�����Ѿ��޸Ĺ�һ�Σ�\r\n");
    }
}