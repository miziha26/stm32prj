#ifndef W25Q128_H
#define W25Q128_H

void W25Q128_SPI_Init(void);
u16 W25Q128_Read_MFID(void);
u32 W25Q128_Read_Bytes(u32 addr, u8 *buf, u32 len);
u32 W25Q128_Write_Bytes(u32 addr,u8* data,u32 len);


#endif



