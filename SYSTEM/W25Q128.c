#include "stm32f10x.h"
#include <stdio.h>

// 片选 低电平优先
#define W25Q128_ENABLE GPIO_ResetBits(GPIOB, GPIO_Pin_12) // CS->0
#define W25Q128_DISABLE GPIO_SetBits(GPIOB, GPIO_Pin_12)  // CS->1

/*
	初始化W25Q128所对应的MCU的SPI控制器 SPI2
	PB12 CS 片选
	PB13 SCK 时钟线
	PB14 MISO   主收从发       SDO 串行数据输出
	PB15 MOSI   主发从收       SDI 串行数据输入
*/
void W25Q128_SPI_Init(void)
{
	// 1.配置对应的GPIO口
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;	// 复用推挽输出
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	// 2.配置SPI控制器
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	SPI_InitTypeDef SPI_InitStruct = {0};
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4; // APB2_CLK = 84M/4=21M W25Q128最高可达104M
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;						// W25Q128支持标准的SPI的00 11 模式 我们这里采用 00模式
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStruct.SPI_CRCPolynomial = 7;							// CRC校验系数只要大于1即可
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;					// 指定以字节为单位
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex; // 全双工
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;					// 高位优先
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;						// 主设备
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;							// NSS由软件控制
	SPI_Init(SPI2, &SPI_InitStruct);
	// 3.使能SPI控制器
	SPI_Cmd(SPI2, ENABLE);
}

/*
	W25Q128_Write_A_Byte:通过SPI2发送一个字节给W25Q128
	@data:要发送的数据
	返回值:void
*/
void W25Q128_Write_A_Byte(u8 data)
{
	//txe为1时代表发送缓冲区为空，rxne为1时代表接收缓冲区不为空
	// 等待发送缓冲器(缓冲区)为空 TXE
    while ((SPI2->SR & SPI_SR_TXE) == 0);
    SPI2->DR = data;

    // 当接收到数据的时候意味着发送完成，只是接收到的数据无效，但还是我们写入的值，只是没用而已
    while ((SPI2->SR & SPI_SR_RXNE) == 0);//等待接收到数据
    (void)SPI2->DR;  // 读取DR以清除RXNE标志，以便后续操作
}

/*
	W25Q128_Read_A_Byte:通过SPI2从W25Q128中读取一个字节的数据
	返回值:读取到的字节
dr发送一个哑字节给w25q128，目的是为了提供时钟信号，然后w25q128返回数据给dr
*/
u8 W25Q128_Read_A_Byte(void)
{
	while ((SPI2->SR & SPI_SR_TXE) == 0);
		SPI2->DR = 0xFF;  // 发送一个dummy字节
	
	while ((SPI2->SR & SPI_SR_RXNE) == 0);
		u8 data = SPI2->DR;  // 返回接收到的数据
	return data;
}

/*
	W25Q128_Get_SR1:获取W25Q128状态寄存器1
*/
u8 W25Q128_Get_SR1(void)
{
	W25Q128_ENABLE;					   // 使能片选 选中芯片
	W25Q128_Write_A_Byte(0x05);		   // 发送读取SR1的指令
	u8 status = W25Q128_Read_A_Byte(); // 接收SR1的值
	W25Q128_DISABLE;				   // 拉高结束
	return status;
}

/*
	W25Q128_Wait_NoBusy:等待W25Q128不忙
*/
void W25Q128_Wait_NoBusy(void)
{
	while (W25Q128_Get_SR1() & 0x01); // sr1中最低位为1时代表忙，0为不忙
}

/*
	W25Q128_Read_MFID:读取厂商ID和设备ID 测试从设备是否有问题
	返回值:厂商ID和设备ID
	发送0x90是为了告诉W25Q128芯片你要读取厂商ID，
	而后面的三个0x00则是根据该指令的格式要求发送的占位符。
	这是为了确保指令在数据传输时完整，以便W25Q128能够正确解析并返回厂商ID的信息。
*/
u16 W25Q128_Read_MFID(void)
{
	W25Q128_ENABLE;
	W25Q128_Write_A_Byte(0x90);
	W25Q128_Write_A_Byte(0x00);
	W25Q128_Write_A_Byte(0x00);
	W25Q128_Write_A_Byte(0x00);
	/*
	W25Q128返回厂商ID和设备ID是分两次返回的。
	首先，它会返回高8位的厂商ID，然后返回低8位的设备ID。
	因此，要调用W25Q128_Read_A_Byte()两次，第一次读取高8位，第二次读取低8位。
	*/
	u16 MFID = W25Q128_Read_A_Byte() << 8; // 厂商ID放在MFID的高8位 0xFF00
	MFID = MFID | W25Q128_Read_A_Byte();   // 设备ID放在MFID的低8位 0XFE
	W25Q128_DISABLE;
	return MFID;
}

/*
	W25Q128_Read_Bytes:读取W25Q128的数据
	返回值:返回实际读取到的字节数
	addr要读取的初始地址，buf存储读取到的数据，len要读取的长度
	W25Q128 的地址是 24 位（3 字节），因为它的最大存储容量为 16MB（2^24 字节）。
	这意味着它需要 24 位的地址空间来唯一标识 0 到 16,777,215 之间的每一个字节。每个地址对应一个存储位置，因此 24 位足够表示整个存储范围。
	在 W25Q128 中，传递的参数是 u32 类型的地址，但实际使用的有效地址是低 24 位（即 u24）。
	因此，虽然参数是 32 位，但高 8 位通常会被忽略，只使用低 24 位进行实际操作。
	不使用u24原因：
	在 C 语言中，没有 u24 这种数据类型，标准的无符号整数类型通常有 u8、u16、u32 和 u64 等。使用 u32 类型的主要原因是：
	数据类型标准化：C 语言的标准数据类型方便移植和使用，32 位无符号整数（u32）可以很容易地与其他系统兼容。
	内存对齐：使用 32 位类型可以更好地利用 CPU 的内存对齐特性，可能提高性能。
	简单性：开发者可以直接使用 32 位的整数进行计算，而不需要担心 24 位整数的特殊处理。
	虽然 W25Q128 的地址只需要 24 位，但使用 u32 可以简化编程，并且更符合 C 语言的标准。使用时，开发者只需关注低 24 位的有效数据。
*/
u32 W25Q128_Read_Bytes(u32 addr, u8 *buf, u32 len)
{
	if (len == 0)
		return 0;
	//w25q128的容量为128Mb,16M字节，即2^24bit,所以要计算一下可以读取的最长字节数
	int max_bytes = (1 << 24) - addr; // 此行代码计算从给定地址 addr 开始，最多能读取的字节数。
	len = (len < max_bytes) ? len : max_bytes;//将要读取的字节数限制在最大可读取字节数 max_bytes 之内。如果请求的 len 大于 max_bytes，则将其设置为 max_bytes。
	int i = len;//将调整后的长度存储在 i 中，以便在最后返回实际读取的字节数。
	W25Q128_ENABLE;
	W25Q128_Write_A_Byte(0x03);//向闪存发送读命令（0x03），指示芯片将进入读取模式。
	W25Q128_Write_A_Byte((addr >> 16) & 0xFF); // 发送24bits的地址 MSB优先 先发[23:16]
	W25Q128_Write_A_Byte((addr >> 8) & 0xFF);  // 发送地址的中间字节（地址的中间 8 位）15:8
	W25Q128_Write_A_Byte(addr & 0xFF);				 // 发送地址的最低字节（地址的低 8 位）7:0
	/*
	发送高位地址的指令后，W25Q128 不会立即返回数据。相反，它会在接收到完整的 24 位地址（即三个字节：高位、中间位和低位）后，
	准备好数据。只有在发送完所有地址字节后，才会开始返回相应的数据。这是为了确保闪存能准确识别要读取的地址。
	*/
	while (len--)
	{
		*buf++ = W25Q128_Read_A_Byte(); // 读取数据
	}
	W25Q128_DISABLE;
	return i;
}

/*
	W25Q128_Write_Enable:写使能
	擦除和写入之前都要写使能，原因如下
	1.保护数据：在擦除和写入操作之前要求写使能，可以防止意外的数据修改。只有在明确启用写入后，设备才允许执行这些敏感的操作。

	2.避免错误操作：写使能命令确保操作的用户确实希望进行写入或擦除。这可以避免在没有意图的情况下修改或删除数据。

	3.状态管理：写使能状态通常在内部被管理，只有在设置了写使能状态后，后续的写入和擦除命令才会被执行。否则，设备会忽略这些命令。
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
	虽然传入的参数是一个 32 位无符号整数（u32），但这个地址是一个 24 位的地址，用于定位 Flash 存储器中的扇区。
	发给w25q128一个扇区的起始地址，W25Q128 将根据这个地址擦除相应的扇区，将所有字节设置为 0xFF。
*/
void W25Q128_Erase_Sector(u32 addr)
{
	W25Q128_Write_Enable(); // 写使能
	W25Q128_ENABLE;
	W25Q128_Write_A_Byte(0x20);
	W25Q128_Write_A_Byte((addr >> 16) & 0xFF); // 发送24bits的地址 MSB优先 先发[23:16]
	W25Q128_Write_A_Byte((addr >> 8) & 0xFF);
	W25Q128_Write_A_Byte(addr & 0xFF);
	W25Q128_DISABLE;
	W25Q128_Wait_NoBusy(); // 等待擦除完成
}

/*
	W25Q128_Page_Program:页写
	spi控制器把data中的数据发送给w25q128之后，w25q128会自动把这些数据一次填入对应的地址中
	@addr:需要写入的地址
	W25Q128 使用一个 24 位地址结构来定位存储器中的数据。具体来说：
	高 8 位（[23:16]）：
	代表的是存储器的高位部分，通常用于指定更大的地址范围或页的分组。
	这部分通常与特定的内存块或区域相关联。
	中间 8 位（[15:8]）：
	也用于定位存储器的区域，但通常是用来指定更小的页或段。
	这部分有助于细化地址，进一步确定数据在存储器中的位置。
	低 8 位（[7:0]）：
	具体表示在当前页内的偏移量，即要写入或读取的具体字节位置。
	页大小为 256 字节，因此可以用 0 到 255 的值表示在该页内的具体位置。
	假设 addr 是 0x00000010，表示第一个页的开始位置。
	低 8 位 addr & 0xFF 的值是 0x10(十进制16)，表示在该页内的偏移量是 16 字节。
*/
void W25Q128_Page_Program(u32 addr, u8 *data, u16 len)
{
	/*
	计算当前页剩余可写入的字节数。W25Q128 的写入页大小为 256 字节，因此通过 (addr & 0xFF) 获取当前地址的低 8 位，得到页内地址。
	max_len 是当前页剩余可写入的长度。
	*/
	int max_len = 256 - (addr & 0xFF); // 低8位为页内地址
	len = len < max_len ? len : max_len;
	W25Q128_Write_Enable(); // 写使能
	W25Q128_ENABLE;
	W25Q128_Write_A_Byte(0x02);//页写命令
	W25Q128_Write_A_Byte((addr >> 16) & 0xFF); // 发送24bits的地址 MSB优先 先发[23:16]
	W25Q128_Write_A_Byte((addr >> 8) & 0xFF);
	W25Q128_Write_A_Byte(addr & 0xFF);
	int i;
	for (i = 0; i < len; i++)
	{
		W25Q128_Write_A_Byte(data[i]);
	}
	W25Q128_DISABLE;
	W25Q128_Wait_NoBusy(); // 等待写入完成
}

/*
	W25Q128_Write_Bytes:往W25Q128中写入多个字节
	需要考虑手动翻页的问题
	先擦除整个扇区再写入？要不要考虑其他的页保存的问题(不能无缘无故去删除其他页的内容)
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
	需要考虑两个问题
	1.写入的内容不足一个擦除空间(扇区)
	2.写入的内容超过一个擦除空间(分多次写入)
*/

u8 buffer[0x1000];
int W25Q128_Write_Bytes(u32 addr, u8 *data, u32 len)
{
	// 判断并更新len
	int max_len = (1 << 24) - addr; // 最多能写入的字节数
	len = (len < max_len) ? len : max_len;
	int write_bytes = 0, i; // 已经写入的字节数
	// 可能存在需要分多次写入的情况 所以每次都需要计算地址
	u32 first_sector_addr = addr & ~0xfff;		  // 计算第一个扇区的起始地址，通过清除 addr 的低 12 位来实现。
	u32 last_sector_addr = (addr + len) & ~0xfff; // 最后一个扇区的地址
	// 一个扇区一个扇区的写 一个扇区的大小为4kb则每一次写入 都需要地址+4K ==> +0x1000
	int sector_addr; // 当前写入的扇区的地址
	u32 addr_offset;
	for (sector_addr = first_sector_addr; sector_addr <= last_sector_addr; sector_addr += 0x1000)
	{
		// 第一个扇区和最后一个扇区需要单独的处理,因为在写入时第一个扇区的偏移量之前可能会有一些数据，所以要保留，
		//同理当写入的数据到达最后一个扇区时，要写入的内容可能不足以覆盖整个扇区，所以也要把扇区之前的数据都保留
		if (sector_addr == first_sector_addr || sector_addr == last_sector_addr)
		{
			W25Q128_Read_Bytes(sector_addr, buffer, 0x1000); // 将扇区的数据读取出来
		}
		// 计算从buffer的哪个位置开始替换(sector_addr中需要替换的位置离首地址的偏移量)
		if (sector_addr == first_sector_addr)
			addr_offset = addr & 0xfff;
		else
			addr_offset = sector_addr & 0xfff;
		for (i = addr_offset; i < 0x1000 && write_bytes < len; i++, write_bytes++) // 替换buffer
		{
			buffer[i] = data[write_bytes];//循环从 addr_offset 开始，用 data 中的数据替换 buffer 中相应位置的数据，直到填满整个扇区或达到写入长度。
		}
		W25Q128_Erase_Sector(sector_addr); // 擦除扇区
		// 写入扇区，只能页写
		u32 page_addr = sector_addr;			   // 当前扇区的第一页的地址
		for (i = 0; i < 16; i++, page_addr += 256) // 一页一页的写，每一个扇区有16ye
		{
			W25Q128_Page_Program(page_addr, &buffer[i * 256], 256); // buffer保存的整个扇区(16页)的数据
		}
	}
	return write_bytes;
}
/*
&buffer[i * 256] 在代码中用于获取 buffer 中特定页的起始地址
buffer 的大小：buffer 是一个 4KB 的缓冲区，可以存储一个扇区的所有数据。
每页大小：在 W25Q128 中，每页的大小是 256 字节。因此，一个扇区包含 16 页（4KB / 256 字节）。
i 的值：i 是在循环中从 0 到 15 的索引，表示当前正在写入的页数。
作用
当 i 为 0 时，&buffer[i * 256] 会返回 &buffer[0]，即指向 buffer 的开始，写入第一页的数据。
当 i 为 1 时，&buffer[i * 256] 会返回 &buffer[256]，指向 buffer 的第二页的开始，写入第二页的数据。
依此类推，直到 i 为 15 时，&buffer[i * 256] 返回 &buffer[3840]，指向最后一页的开始。
结论
因此，&buffer[i * 256] 的作用是为每次页写入提供正确的缓冲区起始地址，使得在写入每页时能够正确地从 buffer 中提取相应的数据。

清除 addr 的低 12 位可以得到扇区的起始地址，是因为每个扇区的大小是 4KB（即2^12字节）。
在计算机中，低 12 位用于表示地址内的偏移量，清除这些位就相当于将地址归零到该扇区的起始位置。
具体解释：
扇区大小：一个扇区的大小为 4KB = 2^12字节。因此，任何地址的低 12 位都表示在该扇区内的偏移量。
按位与操作：通过 addr & ~0xFFF，将 addr 的低 12 位清零，可以得到对应扇区的起始地址。
示例：
假设 addr 是 0x00000ABC：
低 12 位（偏移）为 0xABC，清零后得到 0x00000A00，这就是该扇区的起始地址。
总结：
通过这种方式，可以快速找到某个地址对应的扇区起始位置，而不需要额外的计算。

低 8 位代表页内偏移地址。在 W25Q128 等闪存中，每页的大小通常是 256 字节，因此低 8 位用于表示在当前页内的偏移量。
具体解释：
页大小：每页的大小为 256 字节，即 2^8字节。因此，地址的低 8 位可以表示在当前页内的偏移量。
偏移计算：通过 addr & 0xFF 可以获取地址在当前页内的具体偏移。
示例：
假设 addr 是 0x00000A12：
低 8 位为 0x12，表示在当前页内的偏移量是 18 字节。
总结：
低 8 位用于确定在当前页中的具体位置，而高位则用于确定扇区或块的位置。这种分层地址结构使得在访问闪存时能够高效地定位数据。
*/




