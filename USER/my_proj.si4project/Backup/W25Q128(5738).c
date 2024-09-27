#include "stm32f10x.h"
#include "stm32f10x_spi.h"
#include "DEBUG.h"

#define W25Q128_ENABLE GPIO_ResetBits(GPIOB, GPIO_Pin_12)  //CS->0
#define W25Q128_DISABLE GPIO_SetBits(GPIOB, GPIO_Pin_12)   //CS->1

/*
    初始化W25Q128所对应的MCU的SPI控制器 SPI1
    PB13 (SCK) PB14 (MISO) PB15 (MOSI) PB12 (CS)
*/
void W25Q128_SPI_Init(void)
{
    // 1. 配置对应的GPIO口
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  // STM32F1是APB2
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 配置 SCK (PB13), MISO (PB14), MOSI (PB15) 为复用推挽
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;  // 复用推挽输出
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;  // SPI通信不需要太高速度
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    // 配置 CS (PB12) 为普通推挽输出
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;  // 普通推挽输出
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;  // 一般速度即可
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    // 2. 配置 SPI 控制器
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);  // 使能 SPI1 时钟
    SPI_InitTypeDef SPI_InitStruct = {0};

    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;  // 84MHz/4=21MHz
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;  // 时钟极性和相位配置为模式 0 (CPOL = 0, CPHA = 0)
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStruct.SPI_CRCPolynomial = 7;  // CRC 校验系数
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;  // 8 位数据传输
    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  // 全双工
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;  // MSB 先传
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;  // 主设备
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;  // 软件控制 NSS
    SPI_Init(SPI1, &SPI_InitStruct);

    // 3. 使能 SPI 控制器
    SPI_Cmd(SPI1, ENABLE);
}

/*
	W25Q128_Write_A_Byte:通过SPI1发送一个字节给W25Q128
	@data:要发送的数据
	返回值:void
*/
void W25Q128_Write_A_Byte(u8 data)
{
	//等待发送缓冲器(缓冲区)位空 TXE
	while(SPI_GetFlagStatus(SPI1,SPI_FLAG_TXE)!=SET); 
	SPI_SendData(SPI1,data);
	//当接收到数据的时候意味着发送完成，只是接收到的数据无效的 dummy 
	while(SPI_GetFlagStatus(SPI1,SPI_FLAG_RXNE)!=SET);
    SPI_ReceiveData(SPI1);
}

/*
	W25Q128_Read_A_Byte:通过SPI1从W25Q128中读取一个字节的数据
	返回值:读取到的字节
*/
u8 W25Q128_Read_A_Byte(void)
{
	//等待发送缓冲器(缓冲区)位空 TXE
	while(SPI_GetFlagStatus(SPI1,SPI_FLAG_TXE)!=SET); 
	SPI_SendData(SPI1,0xFF);
	//当接收到数据的时候意味着发送完成，只是接收到的数据无效的 dummy 
	while(SPI_GetFlagStatus(SPI1,SPI_FLAG_RXNE)!=SET);
    return SPI_ReceiveData(SPI1);
}

/*
	W25Q128_Get_SR1:获取W25Q128状态寄存器1
*/
u8 W25Q128_Get_SR1(void)
{
    W25Q128_ENABLE;//使能片选 选中芯片
    W25Q128_Write_A_Byte(0x05);//发送读取SR1的指令
    u8 status = W25Q128_Read_A_Byte();//接收SR1的值
    W25Q128_DISABLE;//拉高结束
    return status;
}

/*
	W25Q128_Wait_NoBusy:等待W25Q128不忙
*/
void W25Q128_Wait_NoBusy(void)
{
	while(W25Q128_Get_SR1()&0x01);//BUSY->0 不忙
}

/*
	W25Q128_Read_MFID:读取厂商ID和设备ID 测试从设备是否有问题
	返回值:厂商ID和设备ID
*/
u16 W25Q128_Read_MFID(void)
{
    W25Q128_ENABLE;
    W25Q128_Write_A_Byte(0x90);
    W25Q128_Write_A_Byte(0x00);
    W25Q128_Write_A_Byte(0x00);
    W25Q128_Write_A_Byte(0x00);
    u16 MFID = W25Q128_Read_A_Byte()<<8;//厂商ID放在MFID的高8位 0xFF00
    MFID = MFID | W25Q128_Read_A_Byte();//设备ID放在MFID的低8位 0XFE
    W25Q128_DISABLE;
    return MFID;
}

/*
	W25Q128_Read_Bytes:读取W25Q128的数据
	返回值:返回实际读取到的字节数
*/
u32 W25Q128_Read_Bytes(u32 addr,u8* buf,u32 len)
{
	if(len == 0) return 0;
	int max_bytes = (1<<24)-addr;//最多能读取的字节数
	len = (len<max_bytes)?len:max_bytes;
    W25Q128_ENABLE;
    W25Q128_Write_A_Byte(0x03);
    W25Q128_Write_A_Byte((addr>>16)&0xFF);//发送24bits的地址 MSB优先 先发[23:16]
    W25Q128_Write_A_Byte((addr>>8)&0xFF);
    W25Q128_Write_A_Byte(addr&0xFF);
 	int i;
 	for(i=0;i<len;)
    {
        buf[i++] = W25Q128_Read_A_Byte();//读取数据
    }
    W25Q128_DISABLE;
    return i;
}

/*
	W25Q128_Write_Enable:写使能
*/
void W25Q128_Write_Enable(void)
{
	W25Q128_ENABLE;
	W25Q128_Write_A_Byte(0x06);
	W25Q128_DISABLE;
}

/*
	W25Q128_Erase_Sector:擦除扇区
	@addr:要擦除的扇区地址
*/
void W25Q128_Erase_Sector(u32 addr)
{
	W25Q128_Write_Enable();//写使能
	W25Q128_ENABLE;
	W25Q128_Write_A_Byte(0x20);
	W25Q128_Write_A_Byte((addr>>16)&0xFF);//发送24bits的地址 MSB优先 先发[23:16]
    W25Q128_Write_A_Byte((addr>>8)&0xFF);
    W25Q128_Write_A_Byte(addr&0xFF);
    W25Q128_DISABLE;
    W25Q128_Wait_NoBusy();//等待擦除完成
}

/*
	W25Q128_Page_Program:页写
	@addr:需要写入的地址
*/
u16 W25Q128_Page_Program(u32 addr,u8* data,u16 len)
{
	int max_len = 256-(addr&0xFF);//低8位为页内地址
	len = len<max_len?len:max_len;
	W25Q128_Write_Enable();//写使能
	W25Q128_Wait_NoBusy();//等待写入完成
	W25Q128_ENABLE;
	W25Q128_Write_A_Byte(0x02);
	W25Q128_Write_A_Byte((addr>>16)&0xFF);//发送24bits的地址 MSB优先 先发[23:16]
    W25Q128_Write_A_Byte((addr>>8)&0xFF);
    W25Q128_Write_A_Byte(addr&0xFF);
    int i;
    for(i=0;i<len;i++)
    {
    	W25Q128_Write_A_Byte(data[i]);
    }
    W25Q128_DISABLE;
    W25Q128_Wait_NoBusy();//等待写入完成
    return i;
}

/*
	W25Q128_Write_Bytes:往W25Q128中写入多个字节
	需要考虑手动翻页的问题
	先查出整个删除再写入？要不要考虑其他的页保存的问题(不能无缘无故去删除其他页的内容)
	先读出来(整个扇区)再修改之后再写入 
	整个扇区4kb
		但是32的栈定义的0x400 1024个字节 似乎保存不下啊 局部变量保存在栈中 
		如果强行的定义了一个超过1024的局部变量，那么编译会通过 但是运行的时候到导致HardFault错误
		去执行HardFault_Handler函数而陷入死循环
		解决问题:1.定义全局变量 2.修改栈的大小Stack_Size 
	写入之前，要先擦除，选择擦除扇区(最小的擦除单位)
	擦除之前，要先写使能
	写入数据的流程
	a.写使能
	b.擦除
		在擦除和写入之前要先判断是否busy，擦除和写入都需要时间
	c.页写
		如果写入的数据不够擦除的数据，为了使得空间其他的数据不变，需要把擦除的空间的数据先
		读取出来，保存到数组中，然后将要写入的数据替换到数组所对应的位置，最后将替换的数组写入
	需要考虑两个为题
	1.写入的内容不足一个擦除空间(扇区)
	2.写入的内容超过一个擦除空间(分多次写入)
*/
u8 buffer[0x1000];
u32 W25Q128_Write_Bytes(u32 addr,u8* data,u32 len)
{
	//判断并更新len
	int max_len = (1<<24)-addr;//最多能写入的字节数
	len = (len < max_len)?len:max_len;
	int write_bytes = 0,i;//已经写入的字节数
	//可能存在需要分多次写入的情况 所以每次都需要计算地址
	u32 first_sector_addr = addr&~0xfff;     //第一个扇区的地址
	u32 last_sector_addr = (addr+len)&~0xfff;//最后一个扇区的地址
	//一个扇区一个扇区的写 一个扇区的大小为4kb则每一次写入 都需要地址+4K ==> +0x1000
	int sector_addr;//当前写入的扇区的地址
	for(sector_addr=first_sector_addr;sector_addr<=last_sector_addr;sector_addr+=0x1000)
	{
		//第一个扇区和最后一个扇区需要单独的处理
		if(sector_addr==first_sector_addr||sector_addr==last_sector_addr)
		{		
			W25Q128_Read_Bytes(sector_addr,buffer,0x1000);//将扇区的数据读取出来
		}
		//计算从buffer的哪个位置开始替换(sector_addr中需要替换的位置离首地址的偏移量)
		if(sector_addr==first_sector_addr)
			u32 addr_offset = addr&0xfff;
		else 
			u32 addr_offset = sector_addr&0xfff;
		for(i=addr_offset;i<0x1000 && write_bytes<len;i++,write_bytes++)//替换buffer 
		{
			buffer[i] = data[write_bytes];
		}	
		W25Q128_Erase_Sector(sector_addr);//擦除扇区
		//写入扇区，只能页写
		u32 page_addr = sector_addr;//当前扇区的第一页的地址
		for(i=0;i<16;i++,page_addr+=256)//一页一页的写，每一个扇区有16ye
		{
			W25Q128_Page_Program(page_addr,&buffer[i*256],256);//buffer保存的整个扇区(16页)的数据
		}
	}
	return write_bytes;
}


