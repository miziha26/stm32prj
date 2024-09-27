#include "stm32f10x.h"
#include "led.h"
/*
	初始化LED灯 D1,PA11;D2,PA12;D3,A15;D4,PB3
	初始化GPIO口-->输出-->开漏-->低速  默认关闭
*/
void Init_Led(void)
{
	//1.使能时钟
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
	//2.初始化
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;//
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	//3.默认关闭
	GPIO_WriteBit(GPIOA, GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_15, Bit_SET);
	GPIO_WriteBit(GPIOB, GPIO_Pin_3, Bit_SET);
}

/*
	控制LED灯
*/
void Led_Ctl(enum LED_NUM led_num,enum LED_STATE led_state)
{
	switch(led_num)
	{
		case LED1:
			GPIO_WriteBit(GPIOA,GPIO_Pin_11, (BitAction)led_state);break;
		case LED2:
			GPIO_WriteBit(GPIOA,GPIO_Pin_12, (BitAction)led_state);break;
		case LED3:
			GPIO_WriteBit(GPIOA,GPIO_Pin_15, (BitAction)led_state);break;
		case LED4:
			GPIO_WriteBit(GPIOB,GPIO_Pin_3, (BitAction)led_state);break;
	}

}




