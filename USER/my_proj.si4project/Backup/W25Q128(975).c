#include "W25Q128.h"
 
u8 W25Q128_ID[2];      //制造商/设备 ID
/*
硬件连接：
CS---->GPIOB_12
SCLK-->GPIOB_13
MOSI-->GPIOB_14
MISO-->GPIOB_15
*/
 
void W25Q128_Init(void)
{
	/*1.开时钟*/
	RCC->APB2ENR |= 1<<3; 
	/*2.配置GPIO口*/
	GPIOB->CRH &= 0x0000FFFF;
	GPIOB->CRH |= 0x83330000;
	/*3.SCLK,CS空闲时为高电平（上拉）*/
	GPIOB->ODR |= 0xF<<12;
	printf("W25Q128 GPIO初始化成功！\n");
}
 
/*
函数功能:  SPI底层时序、读写一个字节（第4种SPI协议 CPOL=1 CPHA=1 ）
*/
u8 W25Q128_SPI_ReadWriteOneByte(u8 tx_data)
{
	u8 rx_data=0,i=0;
	W25Q128_SCLK=1;
	for(i=0;i<8;i++)
	{
		W25Q128_SCLK=0;					
		/*主机发送数据*/
		if(tx_data&0x80) W25Q128_MOSI=1;  //先发高位再发低位
		else W25Q128_MOSI=0;
		tx_data<<=1;
		/*从机接收数据*/									
		rx_data<<=1;											//默认接收为0
		if(W25Q128_MISO) rx_data |= 0x1;  //先接高位再接低位		
		W25Q128_SCLK=1;	
	}
	return rx_data;
}
 
/*
函数功能：在W25Q128指定位置写入指定长度数据（页写）
函数参数：
		addr：要写入W25Q128的起始地址
		p：源数据地址
		len：要写入的数据长度（字节）
*/
void W25Q128_WritePageData(u32 addr ,u8 * p ,u32 len )
{
	u32 i=0;
	W25Q128_WriteEnable(); //W25Q128写使能
	W25Q128_CS=0; //拉低片选脚，选中设备
	W25Q128_SPI_ReadWriteOneByte(0x02);  //页写命令
	W25Q128_SPI_ReadWriteOneByte(addr>>16); //A23~A16 要写入W25Q128的24位地址 （先发高位）
	W25Q128_SPI_ReadWriteOneByte(addr>>8); //A15~A8
	W25Q128_SPI_ReadWriteOneByte(addr); //A7~A0
	for(i=0;i<len;i++)   //必须要写len,不能填256。因为写数据不一定有256个。
	{
			W25Q128_SPI_ReadWriteOneByte(p[i]);
	}
	W25Q128_CS=1; //释放设备
	W25Q128_BusyStateWait();//等待W25Q128页写结束	
}
 
/*
函数功能：在W25Q128指定位置读取指定长度数据
函数参数：
		addr：读取数据源地址
		p：存放读取地址
		len：要读取的数据长度（字节）
*/
void W25Q128_ReadData(u32 addr ,u8 * p ,u32 len )
{
	u32 i=0;
	W25Q128_CS=0; //拉低片选脚，选中设备
	W25Q128_SPI_ReadWriteOneByte(0x03);  //页读取命令
	W25Q128_SPI_ReadWriteOneByte(addr>>16); //A23~A16 要写入W25Q128的24位地址 （先发高位）
	W25Q128_SPI_ReadWriteOneByte(addr>>8); //A15~A8
	W25Q128_SPI_ReadWriteOneByte(addr); //A7~A0
	for(i=0;i<len;i++)  //必须要写len,不能填256。因为读数据不一定读256个。
	{
			p[i]=W25Q128_SPI_ReadWriteOneByte(0xFF);
	}
	W25Q128_CS=1; //释放设备
}
 
/*
函数功能：在W25Q128指定位置写入指定长度数据（没有考虑扇区擦除也没有考虑数据误擦除）
函数参数：
		p：要写入的数据
		addr：写入数据的地址
		len：要写入的数据长度（字节）
*/
 
void W25Q128_WriteDataNoCheck(u32 addr ,u8 * p ,u32 len)
{
	u32  page_tmp;
	page_tmp=256-addr%256; //目标地址页剩余可以写的空间
	if(page_tmp>=len) page_tmp=len;
	while(1)  //写数据
	{
		W25Q128_WritePageData(addr,p,page_tmp);
		if(page_tmp==len) break;  //数据已经全部写入完成
		addr+=page_tmp;    //写地址偏移
		p+=page_tmp; //源数据地址偏移
		len-=page_tmp;  //计算剩下的数据
		if(len>=256)	page_tmp=256;  //计算下一次页写的数据大小
		else	page_tmp=len;
	}
}
 
 
/*
函数功能：在W25Q128指定位置写入指定长度数据（考虑到了数据误擦除）
函数参数：
		p：要写入的数据
		addr：写入数据的地址
		len：要写入的数据长度（字节）
*/
 
void W25Q128_WriteData(u32 addr ,u8 * p ,u32 len)
{
	u8 W25Q128_Buff[4096]; //备份缓冲区
	u32  page_tmp,sector_num,Erase_Len=len,Erase_Addr=addr,buff_len;
	sector_num=addr/4096; //计算当前扇区数
	buff_len=addr-sector_num*4096;
//	printf("sector_num=%d buff_len=%d \n",sector_num,buff_len);
	
	page_tmp=256-addr%256; //目标地址页剩余可以写的空间
	if(page_tmp>=len) page_tmp=len;
	
	W25Q128_ReadData(sector_num*4096,W25Q128_Buff,buff_len); //备份源扇区的数据
	
	while(1) //以扇区的方式擦除要写数据的地方
	{
		if(Erase_Len<=4096-Erase_Addr%4096)
		{
			W25Q128_SectorErase(Erase_Addr,0x20);
			break;
		}
		else if(Erase_Len) //如果擦除的空间不够写入数据，继续擦除空间
		{
			W25Q128_SectorErase(Erase_Addr,0x20); 
			Erase_Addr+=4096; //擦除地址偏移
			Erase_Len-=4096;  //还剩下待擦除的空间大小
		}
		else 
		{
			break;
		}
	}
	W25Q128_WriteDataNoCheck(sector_num*4096,W25Q128_Buff,buff_len); //还原之前备份的数据 
	while(1)  //写数据
	{
		W25Q128_WritePageData(addr,p,page_tmp);
		if(page_tmp==len) break;  //数据已经全部写入完成
		addr+=page_tmp;    //写地址偏移
		p+=page_tmp; //源数据地址偏移
		len-=page_tmp;  //计算剩下的数据
		if(len>=256)	page_tmp=256;  //计算下一次页写的数据大小
		else	page_tmp=len;
	}
}
 
/*
函数功能： 擦除W25Q128指定扇区
函数参数：
				addr：要擦除W25Q128的地址
说明：擦除命令不能字节擦除，只能块/扇区擦除（一个扇区4096字节）
			0~4095 ：第一个扇区
			4096~..：第二个扇区
			.....  : ...
 
块擦除(64KB) D8h 
块擦除(32KB) 52h
扇区擦除(4KB) 20h
*/
void W25Q128_SectorErase(u32 addr,u8 cmd)
{
	W25Q128_WriteEnable(); //W25Q128写使能
	W25Q128_CS=0; //拉低片选脚，选中设备
	W25Q128_SPI_ReadWriteOneByte(cmd); //擦除扇区命令
	W25Q128_SPI_ReadWriteOneByte(addr>>16); //A23~A16 要擦除W25Q128的24位地址 （先发高位）
	W25Q128_SPI_ReadWriteOneByte(addr>>8); //A15~A8
	W25Q128_SPI_ReadWriteOneByte(addr); //A7~A0
	W25Q128_CS=1; //释放设备
	W25Q128_BusyStateWait();//等待W25Q128擦除扇区接结束
}
 
/*
函数功能:等待 W25Q128直到为空闲状态
*/
void W25Q128_BusyStateWait(void)
{
	u8 tmp=1;
	u16 cnt=0;	
	while(tmp&0x01) 
	{
		W25Q128_CS=0; //拉低片选脚，选中设备
		W25Q128_SPI_ReadWriteOneByte(0x05); //读状态寄存器命令
		tmp=W25Q128_SPI_ReadWriteOneByte(0xFF);  //读状态寄存器
		W25Q128_CS=1; //释放设备
		DelayMs(1);
		if(cnt++>=500) break;  //防止卡死
	} 
}
 
/*
函数功能: W25Q128写使能
*/	
void W25Q128_WriteEnable(void)
{
	W25Q128_CS=0; //拉低片选脚，选中设备
	W25Q128_SPI_ReadWriteOneByte(0x06);  //写使能
	W25Q128_CS=1; //释放设备
}
 
/*函数功能：读取制造商/芯片 ID
制造商ID= EF
设备ID= 17   (W25Q64 的是16H)
*/
void W25Q128_Read_ID(void)
{
	W25Q128_CS=0;												//拉低片选脚，选中设备
	W25Q128_SPI_ReadWriteOneByte(0x90); //读取制造商/设备 ID 指令
 	W25Q128_SPI_ReadWriteOneByte(0x00); 
	W25Q128_SPI_ReadWriteOneByte(0x00);
	W25Q128_SPI_ReadWriteOneByte(0x00); 
	W25Q128_ID[0] = W25Q128_SPI_ReadWriteOneByte(0xFF); //0xFF不是一个指令 只是为了不让函数报错
	W25Q128_ID[1] = W25Q128_SPI_ReadWriteOneByte(0xFF);
	W25Q128_CS=1;												//释放片选脚
	printf("制造商ID= %X\n设备ID= %X \n",W25Q128_ID[0],W25Q128_ID[1]);
}
 
