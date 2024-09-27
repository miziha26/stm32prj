#include"beep.h"
#include "stm32f10x.h"

void Beep_Init(void)
{	//使能时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	//初始化
	GPIO_InitTypeDef GPIO_InitStruct;
	//配置为推挽输出模式
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	//初始化引脚PF8
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	//应用设置
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	//初始状态为关闭
	GPIO_ResetBits(GPIOA,GPIO_Pin_8);
}
void Beep_Toggle(void)
{
    // 读取当前引脚状态并翻转
    GPIOA->ODR ^= GPIO_Pin_8;
}





