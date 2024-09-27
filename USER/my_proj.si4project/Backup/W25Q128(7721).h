#ifndef W25Q128_H
#define W25Q128_H
#include "stm32f10x.h"
#include "stdio.h"
#include "sys.h"
#include "delay.h"
 
#define W25Q128_CS   PBout(12)
#define W25Q128_SCLK PBout(13)
#define W25Q128_MOSI PBout(14)
#define W25Q128_MISO PBin(15)
extern u8 W25Q128_ID[2]; //制造商/设备 ID 
void W25Q128_Init(void);
u8 W25Q128_SPI_ReadWriteOneByte(u8 tx_data);
void W25Q128_WritePageData(u32 addr ,u8 * p ,u32 len );
void W25Q128_ReadData(u32 addr ,u8 * p ,u32 len );
void W25Q128_WriteDataNoCheck(u32 addr ,u8 * p ,u32 len);
void W25Q128_WriteData(u32 addr ,u8 * p ,u32 len);
void W25Q128_SectorErase(u32 addr,u8 cmd);
void W25Q128_BusyStateWait(void);
void W25Q128_WriteEnable(void);
void W25Q128_Read_ID(void);
#endif
 
 
