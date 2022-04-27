#include "RF_A7205.h"
#include "main.h"
//===================================================================================
// A7205 register address
//===================================================================================
#define A7205_MODE_REG            0x00
#define A7205_MODECTRL_REG        0x01
#define A7205_CALIBRATION_REG     0x02
#define A7205_FIFO1_REG           0x03
#define A7205_FIFO2_REG           0x04
#define A7205_FIFO_REG            0x05
#define A7205_IDCODE_REG          0x06
#define A7205_RCOSC1_REG          0x07
#define A7205_RCOSC2_REG          0x08
#define A7205_RCOSC3_REG          0x09
#define A7205_CKO_REG             0x0A
#define A7205_GIO1_REG            0x0B
#define A7205_GIO2_REG            0x0C
#define A7205_CLOCK_REG           0x0D
#define A7205_DATARATE_REG        0x0E
#define A7205_PLL1_REG            0x0F
#define A7205_PLL2_REG            0x10
#define A7205_PLL3_REG            0x11
#define A7205_PLL4_REG            0x12
#define A7205_PLL5_REG            0x13
#define A7205_TX1_REG             0x14
#define A7205_TX2_REG             0x15
#define A7205_DELAY1_REG          0x16
#define A7205_DELAY2_REG          0x17
#define A7205_RX_REG              0x18
#define A7205_RXGAIN1_REG         0x19
#define A7205_RXGAIN2_REG         0x1A
#define A7205_RXGAIN3_REG         0x1B
#define A7205_RXGAIN4_REG         0x1C
#define A7205_RSSI_REG            0x1D
#define A7205_ADC_REG             0x1E
#define A7205_CODE1_REG           0x1F
#define A7205_CODE2_REG           0x20
#define A7205_CODE3_REG           0x21
#define A7205_IFCAL1_REG          0x22
#define A7205_IFCAL2_REG          0x23
#define A7205_VCOCCAL_REG         0x24
#define A7205_VCOCAL1_REG         0x25
#define A7205_VCOCAL2_REG         0x26
#define A7205_BATTERY_REG         0x27
#define A7205_TXTEST_REG          0x28
#define A7205_RXDEM1_REG          0x29
#define A7205_RXDEM2_REG          0x2A
#define A7205_CPC_REG             0x2B
#define A7205_CRYSTALTEST_REG     0x2C
#define A7205_PLLTEST_REG         0x2D
#define A7205_VCOTEST1_REG        0x2E
#define A7205_VCOTEST2_REG        0x2F
#define A7205_IFAT_REG            0x30
#define A7205_RSCALE_REG          0x31
#define A7205_FILTERTEST_REG      0x32
#define A7205_TMV_REG             0x33

//===================================================================================
// A7205 strobe command
//===================================================================================
#define CMD_SLEEP           0x80    //1000,xxxx SLEEP mode
#define CMD_IDLE            0x90    //1001,xxxx IDLE mode
#define CMD_STBY            0xA0    //1010,xxxx Standby mode
#define CMD_PLL             0xB0    //1011,xxxx PLL mode
#define CMD_RX              0xC0    //1100,xxxx RX mode                                                          
#define CMD_RFR             0xF0    //1111,xxxx RX FIFO reset



unsigned char RxCnt;
unsigned char Err_BitCnt;
unsigned char tmpbuf[20];

USART_HandleTypeDef husart1;

const Uint8  BitCount_Tab[16] = {0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4};

Uint8  ID_Tab[4]={0xAA,0xBB,0xCC,0xDD};   //ID code{0x34,0x75,0xC5,0x2A}; //ID code
//Uint8  ID_Tab[4]={0x11,0x22,0x33,0x44};

const Uint8  PN9_Tab[]=
{   0xFF,0x83,0xDF,0x17,0x32,0x09,0x4E,0xD1,
    0xE7,0xCD,0x8A,0x91,0xC6,0xD5,0xC4,0xC4,
    0xC6,0xD5,0xC4,0xC4
};  // This table are 64bytes PN9 pseudo random code.

const Uint8 A7205Config[]=
{
    //      address   name
    //      -------   ----
    0x00,   //0x00  ; A7205_MODE_REG
    0x62,   //0x01  ; A7205_MODE_CONTROL_REG
    0x00,   //0x02  ; A7205_CALIBRATION_REG
    0x10,   //0x3F,   //0x03  ; A7205_FIFO1_REG
    0x40,   //0x04  ; A7205_FIFO2_REG
    0x00,   //0x05  ; A7205_FIFO_REG
    0x00,   //0x06  ; A7205_IDCODE_REG
    0x00,   //0x07  ; A7205_RCOSC1_REG
    0x00,   //0x08  ; A7205_RCOSC2_REG
    0x00,   //0x09  ; A7205_RCOSC3_REG
    0x00,   //0x0A  ; A7205_CKO_REG
    0x01,   //0x0B  ; A7205_GIO1_REG
    0x01,   //0x0C  ; A7205_GIO2_REG
    0x05,   //0x0D  ; A7205_CLOCK_REG
    0x00,   //0x0E  ; A7205_DATARATE_REG  0x18 = 20K
    0x50,   //0x0F  ; A7205_PLL1_REG
    0x9E,   //0x10  ; A7205_PLL2_REG
    0x4B,   //0x11  ; A7205_PLL3_REG
    0x00,   //0x12  ; A7205_PLL4_REG
    0x02,   //0x13  ; A7205_PLL5_REG
    0x16,   //0x14  ; A7205_TX1_REG
    0x2B,   //0x15  ; A7205_TX2_REG
    0x12,   //0x16  ; A7205_DELAY1_REG
    0x40,   //0x17  ; A7205_DELAY2_REG
    0x62,   //0x18  ; A7205_RX_REG
    0x80,   //0x19  ; A7205_RXGAIN1
    0x80,   //0x1A  ; A7205_RXGAIN2
    0x00,   //0x1B  ; A7205_RXGAIN3
    0x0A,   //0x1C  ; A7205_RXGAIN4
    0x32,   //0x1D  ; A7205_RSSI_REG
    0x03,   //0x1E  ; A7205_ADC_REG
    0x05,   //0x1F  ; A7205_CODE1_REG
    0x12,   //0x20  ; A7205_CODE2_REG  0x12时，不允许ID CODE有错  0x16,   //0x20  ; A7205_CODE2_REG
    0x00,   //0x21  ; A7205_CODE3_REG
    0x00,   //0x22  ; A7205_IFCAL1_REG
    0x00,   //0x23  ; A7205_IFCAL2_REG
    0x00,   //0x24  ; A7205_VCOCCAL_REG
    0x00,   //0x25  ; A7205_VCOCAL1_REG
    0x23,   //0x26  ; A7205_VCOCAL2_REG
    0x70,   //0x27  ; A7205_BATTERY_REG
    0x17,   //0x28  ; A7205_TXTEST_REG
    0x47,   //0x29  ; A7205_RXDEM1_REG
    0x80,   //0x2A  ; A7205_RXDEM2_REG
    0x57,   //0x2B  ; A7205_CPC_REG
    0x01,   //0x2C  ; A7205_CRYSTALTEST_REG
    0x45,   //0x2D  ; A7205_PLLTEST_REG
    0x19,   //0x2E  ; A7205_VCOTEST1_REG
    0x00,   //0x2F  ; A7205_VCOTEST2_REG
    0x01,   //0x30  ; A7205_IFAT_REG
    0x0F,   //0x31  ; A7205_RSCALE_REG
    0x00,   //0x32  ; A7205_FILTERTEST_REG
    0x7F,   //0x33  ; A7205_TMV_REG
};


/**********************************************************
* Delay1ms @12MHz
**********************************************************/
void Delay1ms(Uint8 n)
{
    Uint8 i;

    while(n--)
        for(i=0;i<250;i++);
}

/**********************************************************
* Delay100us  @12MHz
**********************************************************/
void Delay100us(Uint8 n)
{
    Uint8 i;
    while(n--)
        for(i=0;i<23;i++);
}

/************************************************************************
**  A7205_Reset_RF
************************************************************************/
void A7205_Reset(void)
{
	WriteReg(A7205_MODE_REG, 0x00); //reset RF chip
}

	Uint8 d1,d2,d3,d4;
/************************************************************************
**  A7205_WriteID
************************************************************************/
void A7205_WriteID(void)
{
	Uint8 i;

	Uint8 addr;

	addr = A7205_IDCODE_REG;   //send address 0x06, bit cmd=0, r/w=0
//	SCS = 0;
	Clr_SCS;
	Delay100us(20);
	
	ByteSend(addr);   //0x06
	for(i=0 ; i<4 ; i++)
		ByteSend(ID_Tab[i]);
		//SCS = 1;
	Set_SCS;
    //for check
	addr = A7205_IDCODE_REG | 0x40; //send address 0x06, bit cmd=0, r/w=1   (0x46)
	
Delay100us(20);
	
//	SCS=0;
	Clr_SCS;
	
Delay100us(20);

	ByteSend(addr); //0x46
	d1=ByteRead();
	d2=ByteRead();
	d3=ByteRead();
	d4=ByteRead();
	/*if ((d1 == ID_Tab[0]) &&(d2 == ID_Tab[1]) &&(d3 == ID_Tab[2]) &&(d4 == ID_Tab[3]))
	{
		_pa7 = 1;
	}
	else
	{
	  _pa7 = 0;	
	}*/
//		SCS=1;
	Set_SCS;

}



/************************************************************************
**  WriteReg
************************************************************************/
void WriteReg(Uint8 addr, Uint8 dataByte)
{
			Uint8 i;

	//	SCS = 0;
			Clr_SCS;
Delay100us(20);	
    addr |= 0x00; //bit cmd=0,r/w=0
    for(i = 0; i < 8; i++)
    {
        if(addr & 0x80)
//            SDIOA = 1;
					Set_SDIOA;
        else
//            SDIOA = 0;
					Clr_SDIOA;
				
 Delay100us(20);
       // SCK = 1;
				Set_SCK;
Delay100us(20);
				
				
//        SCK = 0;
				Clr_SCK;
				
Delay100us(20);
        addr = addr << 1;
    }


    //send data byte
    for(i = 0; i < 8; i++)
    {
        if(dataByte & 0x80)
//            SDIOA = 1;
				Set_SDIOA;
        else
//            SDIOA = 0;
				Clr_SDIOA;
				
Delay100us(20);
				
				
//        SCK = 1;
				Set_SCK;
				
Delay100us(20);							
				
        //SCK = 0;
				Clr_SCK;
Delay100us(20);
        dataByte = dataByte<< 1;
    }
    //SCS = 1;
    Set_SCS;
}


/************************************************************************
**  ReadReg
************************************************************************/
Uint8 ReadReg(Uint8 addr)
{
    Uint8 i;
    Uint8 tmp;
//	Set_SDIO_Out;
		Set_SDIO_OutX();  //设置PB1

Delay100us(20);			
		
	//	SCS = 0;
		Clr_SCS;
Delay100us(20);		
    addr |= 0x40; //bit cmd=0,r/w=1
    for(i = 0; i < 8; i++)
    {

        if(addr & 0x80)
//            SDIOA = 1;
					Set_SDIOA;
				
        else
//            SDIOA = 0;
				Clr_SDIOA;
				
Delay100us(20);			
				
//        SCK = 1;
				Set_SCK;
				
		Delay100us(20);
//        SCK = 0;
				Clr_SCK;
				
	Delay100us(20);							
				
        addr = addr << 1;
    }

		__nop();  

//	Set_SDIO_In;
		Set_SDIO_InX();
		
		__nop();
		__nop();
		__nop();			
		
    //read data
    for(i = 0; i < 8; i++)
    {
        if(SDIO_in)
            tmp = (tmp << 1) | 0x01;
        else
            tmp = tmp << 1;

//        SCK = 1;
				Set_SCK;
				
				
Delay100us(20);					
        //SCK = 0;
				Clr_SCK;
				
Delay100us(20);							
    }
//    SCS = 1;
		Set_SCS;
//    Set_SDIO_Out;
		Set_SDIO_OutX();
    return tmp;
}



/************************************************************************
**  ByteSend
************************************************************************/
void ByteSend(Uint8 src)
{
    Uint8 i;
	
			__nop();
			__nop();	
	
    for(i = 0; i < 8; i++)
    {
				__nop();
        if(src & 0x80)
//            SDIOA = 1;
				Set_SDIOA;
        else
            //SDIOA = 0;
				Clr_SDIOA;
				
Delay100us(1);		
				
        //SCK = 1;
				Set_SCK;
Delay100us(1);					
				
        //SCK = 0;
				Clr_SCK;
				
Delay100us(1);			 				
				
        src = src << 1;
    }
}


/************************************************************************
**  ByteRead
************************************************************************/
Uint8 ByteRead(void)
{
	Uint8 i,tmp;

   // SDIO = 1; //sdio pull high
   	//Set_SDIO_In;
	Set_SDIO_InX();
	//Delay100us(20);
    for(i = 0; i < 8; i++)
    {

				
	
				
				//SCK = 1;
				Set_SCK;
				
				Delay100us(1);						
        if(SDIO_in)
            tmp = (tmp << 1) | 0x01;
        else
            tmp = tmp << 1;

				Delay100us(1);		
        //SCK = 0;
				Clr_SCK;
				Delay100us(1);
    }
    //Set_SDIO_Out;
		Set_SDIO_OutX();
    return tmp;
}


/************************************************************************
**  Send4Bit
************************************************************************/
void Send4Bit(Uint8 src)
{
    Uint8 i;

    for(i = 0; i < 4; i++)
    {
        if(src & 0x80)
            //SDIOA = 1;
				Set_SDIOA;
        else
            //  SDIOA = 0;
				Clr_SDIOA;
				
				Delay100us(1);						
//        SCK = 1;
				Set_SCK;
				Delay100us(1);	
        //SCK = 0;
				Clr_SCK;
				Delay100us(1);			
        src = src << 1;
    }
}


/*********************************************************************
** A7205_SetCH
*********************************************************************/
void A7205_SetCH(Uint8 ch)
{
    //RF freq = RFbase + (CH_Step * ch)
	WriteReg(A7205_PLL1_REG, ch);
}


/*********************************************************************
** A7205_initRF
*********************************************************************/
void A7205_initRF(void)
{
    //init io pin
//    SCS = 1;
//    SCK = 0;
//    SDIOA = 1;
	Set_SCS;
	Clr_SCK;
	Set_SDIOA;
	
  //  CKO = 1;
	//  GIO1 = 1;
  //  GIO2 = 1;

  A7205_Reset(); //reset A7205 RF chip
	A7205_WriteID(); //write ID code
	A7205_Config(); //config A7205 chip
	A7205_Cal(); //calibration IF,vco, vcoc
	
	StrobeCmd(CMD_PLL);
  //A7205_SetCH(100); //freq 2450.001MHz
	A7205_SetCH(156); //freq 2478MHz	
	
	StrobeCmd(CMD_RX); //entry rx		初始化完成后，进入接收模式
	Delay1ms(20);   //20
}



/*********************************************************************
** Strobe Command
*********************************************************************/
void StrobeCmd(Uint8 cmd)
{
	//SCS = 0;
	Clr_SCS;
	Send4Bit(cmd);
	//SCS = 1;
	Set_SCS;
}

/*********************************************************************
** A7205_RxPacket
*********************************************************************/
void A7205_RxPacket(void)
{
    Uint8 i;
    Uint8 recv, tmp;
    Uint8 cmd;

	RxCnt++;
	cmd = A7205_FIFO_REG | 0x40;   //address 0x05, bit cmd=0, r/w=1

    //SCS=0;
	Clr_SCS;
	ByteSend(cmd);

	for(i=0; i <17; i++)
		{
				recv = ByteRead();
				tmpbuf[i]=recv;
			/*
				if((recv ^ PN9_Tab[i])!=0)
  		      	{
  		          	tmp = recv ^ PN9_Tab[i];
  		          	Err_BitCnt += (BitCount_Tab[tmp>>4] + BitCount_Tab[tmp & 0x0F]);
  		      	}*/
	    }
	
	//SCS=1;
			Set_SCS;      //置高
//	if(tmpbuf[11]==0xaa)
//	{
//		for(i=0;i<4;i++)	
//		{
//			ID_Tab[i]=tmpbuf[i];			
//		}
//		A7205_WriteID();
//		
//		printf("\n");
//		for(i=0;i<4;i++)
//		{
//					USART_GetFlagStatus(USART1,USART_FLAG_TC);
//					USART_SendData(USART1, ID_Tab[i]);//向串口1发送数据
//					while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
//		}
//		printf("\n");
//	}

	StrobeCmd(CMD_RX); //entry rx	接收完成后，进入接收模式
	
	//Delay1ms(20);   //20
	//	StrobeCmd(CMD_RX); //entry rx	接收完成后，进入接收模式
	/*串口打印 接收值*/
////////////	for(i=0;i<17;i++)
////////////	{
////////////		    USART_GetFlagStatus(USART1,USART_FLAG_TC);
////////////				USART_SendData(USART1, tmpbuf[i]);//向串口1发送数据
////////////				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
////////////	}
////////////	printf("\r\n");//插入换行

////	HAL_USART_Transmit(&husart1,(uint8_t *)(tmpbuf + i) ,17,3);
	
		for(i=0;i<17;i++)
		{
			    HAL_USART_Transmit(&husart1,(uint8_t *)(tmpbuf + i) ,1,3);
//				HAL_USART_Transmit_IT	(&husart1,(uint8_t *)(tmpbuf + i) ,1);
//				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);   //等待发送结束
		}
	
}


/*********************************************************************
** Err_State
*********************************************************************/
void Err_State(void)
{
    //ERR display
    //Error Proc...
    //...
    while(1);
}


/*********************************************************************
** A7205_calibration
*********************************************************************/
void A7205_Cal(void)
{
	Uint8 tmp;
	Uint8 fb,fbcf,fcd,vb,vbcf,vcb,vccf;
	Uint8 rh,rl;


	//calibration procedure
	StrobeCmd(CMD_STBY);

	WriteReg(A7205_CALIBRATION_REG, 0x0F); //for IF calibration @STB state
	do
	{
		tmp = ReadReg(A7205_CALIBRATION_REG);
		tmp &= 0x0F;
	}
	while (tmp);


	//for check
	//IF calibration
	tmp = ReadReg(A7205_IFCAL1_REG);
	fb = tmp & 0x0F;
	fbcf = (tmp >>4) & 0x01;

	tmp = ReadReg(A7205_IFCAL2_REG);
	fcd = tmp & 0x1F;

	//VCO single band calibration
  tmp = ReadReg(A7205_VCOCAL1_REG);
	vb = tmp & 0x07;
	vbcf = (tmp >>3) & 0x01;

	//VCO current calibration
	tmp = ReadReg(A7205_VCOCCAL_REG);
  vcb = tmp & 0x0F;
	vccf= (tmp >> 4) & 0x01;

	//RSSI calibration
	tmp = ReadReg(A7205_RXGAIN2_REG);
	rh = tmp;

  tmp = ReadReg(A7205_RXGAIN3_REG);
	rl = tmp;

	if (vbcf || fbcf || vccf)
	    Err_State(); //error

}


/*********************************************************************
** A7205_Config
*********************************************************************/
void A7205_Config(void)
{
	Uint8 i;

    for (i=0x01; i<=0x04; i++)
	    WriteReg(i, A7205Config[i]);

		for (i=0x07; i<=0x33; i++)
			WriteReg(i, A7205Config[i]);

}



