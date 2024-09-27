#ifndef  W25Q128_H
#define  W25Q128_H
#include "stm32f10x.h"

void W25Q128_SPI_Init(void);
u16 W25Q128_Read_MFID(void);
void W25Q128_Erase_Sector(u32 addr);
u16 W25Q128_Page_Program(u32 addr,u8* data,u16 len);
u32 W25Q128_Read_Bytes(u32 addr,u8* buf,u32 len);
u32 W25Q128_Write_Bytes(u32 addr,u8* data,u32 len);


#endif 

