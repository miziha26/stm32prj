#include "stm32f10x.h"


/*
	delay_ms:使用systick定时器实现ms级别的延时
	@n:延时多少ms
	返回值为空
*/
void delay_ms(uint16_t n)//n<798
{
	uint16_t a,b,k=500,i;
	a = n/k;//有多少个500
	b = n%k;//还剩多少
	a = b?(a+1):a;
	for(i=0;i<a;i++)
	{
		//最后一次延时的时间
		if(i==a-1&&b) k=b;
		//选择时钟源 默认选择外部时钟源 STCLK 168M/8
		SysTick->LOAD = SystemCoreClock/8/1000*k-1;//设置重装值 
		SysTick->VAL = 0X00;//把当前计数值清空
		//开始计时 使能systick CTRL(控制及状态寄存器的第0位置1)
		SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;//0X01
		//等待定时器溢出  CTRL的第16位置1
		uint32_t tmp;
		do 
		{
			tmp = SysTick->CTRL;
		}
		while(tmp&0x01 && !(tmp&(1<<16)));//等待溢出 或者 提前关闭了定时器
		//关闭定时器
		SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
		SysTick->VAL = 0X00;//如果是提前关闭定时器，需要重新把CNT清零
	}
}

void delay_us(uint16_t n)//n<798
{
	//选择时钟源 默认选择外部时钟源 STCLK 168M/8
	SysTick->LOAD = SystemCoreClock/8/1000000*n-1;//设置重装值 
	SysTick->VAL = 0X00;//把当前计数值清空
	//开始计时 使能systick CTRL(控制及状态寄存器的第0位置1)
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;//0X01
	//等待定时器溢出  CTRL的第16位置1
	uint32_t tmp;
	do 
	{
		tmp = SysTick->CTRL;
	}
	while(tmp&0x01 && !(tmp&(1<<16)));//等待溢出 或者 提前关闭了定时器
	//关闭定时器
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	SysTick->VAL = 0X00;//如果是提前关闭定时器，需要重新把CNT清零
}


/*
	delay_ms_Int:使用systick定时器中断来实现ms级别的延时
	@n:延时多少ms
	返回值为空
*/
uint16_t delay_time;


void delay_ms_Int(uint16_t n)
{
	//配置SYSTICK定时器1ms产生一个中断
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;//禁用systick
	SysTick->CTRL |= 1<<2;//选择内核时钟源
	SysTick->CTRL |= 1<<1;//使能systick中断
	SysTick->LOAD = 168000-1;//N值
	SysTick->VAL = 0X00;//当前计数值为0
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;//使能systick
	delay_time = n;
	while(delay_time);
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}










