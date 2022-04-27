#ifndef __RF_A7205_H__
#define __RF_A7205_H__




#define Uint8		unsigned char 
/*
#define SCS         _pb7        //spi SCS
#define SCK         _pc4        //spi SCK
#define SDIO        _pc5        //spi SDIO

#define Set_SDIO_In		_pcc5 = 1	//SDIO 为输入
#define Set_SDIO_Out	_pcc5 = 0  //设置SDIO为输出

*/

	 
#define GIO1			HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)   //	PFin(3)

#define Clr_SCS   HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_RESET)
#define Set_SCS		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_SET)

#define Set_SCK		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET)
#define Clr_SCK		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET)

#define Set_SDIOA		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET)
#define Clr_SDIOA		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET)

#define SDIO_in   HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)


//#define SCS       PBout(3)  //  PFout(2)        //spi SCS
//#define SCK       PBout(0)  //  PFout(1)        //spi SCK
//#define SDIOA     PBout(1)  //   PFout(0)        //spi SDIO 输出
//#define SDIO_in   PBin(1)   //      PFin(0)         //spi SDIO 输入

//#define Set_SDIO_In		{GPIOB->CRL&=0XFFFFFFF0;GPIOB->CRL|=(u32)8<<0;GPIOB->ODR|=1<<0;}	//SDIO 为输入,上拉
//#define Set_SDIO_Out	{GPIOB->CRL&=0XFFFFFFF0;GPIOB->CRL|=(u32)3<<0;GPIOB->ODR|=0<<0;}  //设置SDIO为输出，高


extern void A7205_Reset(void);
extern void A7205_WriteID(void);
extern void WriteReg(Uint8 addr, Uint8 dataByte);
extern Uint8 ReadReg(Uint8 addr);
extern void ByteSend(Uint8 src);
extern Uint8 ByteRead(void);
extern void Send4Bit(Uint8 src);
extern void A7205_initRF(void);
extern void StrobeCmd(Uint8 cmd);
extern void A7205_RxPacket(void);
extern void Err_State(void);
extern void A7205_Cal(void);
extern void A7205_Config(void);

#endif   

