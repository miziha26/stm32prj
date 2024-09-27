#include "key.h"
#include "stm32f10x.h"

/*
	按键的初始化
	KEY0  PA7 
	KEY1  PA6 
	KEY2  PA1 
	KEY3  PA0 
*/

void Key_Init(void)//PA7 PA6 PA1 PA0
{
	//1.使能时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	//2.初始化
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;	 
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_1 | GPIO_Pin_0;// 初始化引脚 
	GPIO_Init(GPIOE,&GPIO_InitStruct);// 应用设置
}
/*判断按键 并返回按键的状态*/
enum KEY_STATE Key_Status(enum KEY_NUM key_num)
{
	switch(key_num)
	{
		case KEY0:
            // 读取PA7引脚状态
            return (enum KEY_STATE)GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7); 
        case KEY1:
            // 读取PA6引脚状态
            return (enum KEY_STATE)GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6); 
        case KEY2:
            // 读取PA1引脚状态
            return (enum KEY_STATE)GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1);
		case KEY3:
            // 读取PA0引脚状态
            return (enum KEY_STATE)GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);
        default:
            // 返回一个错误状态
            return KEY_ERROR;
	}
}







