#include "exti.h"
#include "stm32f10x.h"

/*
	初始化key0所对应的外部中断 PE4 -- EXTI_Line4
	按下按键高电平->低电平 下降沿触发
*/


//void Init_EXTI_Key(void)
//{
//	//1.使能时钟
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_EXTIT|RCC_APB2Periph_SYSCFG,ENABLE);
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOA,ENABLE);
//	//2.配置GPIO为输入模式
//	GPIO_InitTypeDef GPIO_InitStruct;
//	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
//	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_3|GPIO_Pin_2;
//	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
//	GPIO_Init(GPIOE, &GPIO_InitStruct);
//	
//	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
//	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
//	GPIO_Init(GPIOA,&GPIO_InitStruct);
//	//3.配置SYSCFG
//	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,EXTI_PinSource4);
//	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,EXTI_PinSource3);
//	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,EXTI_PinSource2);
//	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource0);
//	//4.配置外部中断
//	EXTI_InitTypeDef EXTI_InitStruct;
//	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
//	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;//中断模式
//	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;//下降沿触发
//	//配置EXIT Line4,3,2
//	EXTI_InitStruct.EXTI_Line = EXTI_Line4|EXTI_Line3|EXTI_Line2;
//	EXTI_Init(&EXTI_InitStruct);
//	//配置EXIT Line0
//	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;//上升沿触发
//	EXTI_InitStruct.EXTI_Line = EXTI_Line0;
//	EXTI_Init(&EXTI_InitStruct);
//	//5.配置NVIC
//	NVIC_InitTypeDef NVIC_InitStruct;
//	NVIC_InitStruct.NVIC_IRQChannel = EXTI4_IRQn;
//	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 4;
//	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
//	NVIC_Init(&NVIC_InitStruct);
//	NVIC_InitStruct.NVIC_IRQChannel = EXTI3_IRQn;
//	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 3;
//	NVIC_Init(&NVIC_InitStruct);
//	NVIC_InitStruct.NVIC_IRQChannel = EXTI2_IRQn;
//	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
//	NVIC_Init(&NVIC_InitStruct);
//	NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
//	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
//	NVIC_Init(&NVIC_InitStruct);
//}


void Init_EXTI_Key(void)
{
    // 1. 使能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA, ENABLE);

    // 2. 配置GPIO为输入模式
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;  // 输入模式，上拉
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_6 | GPIO_Pin_1 | GPIO_Pin_0;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 3. 配置AFIO (EXTI 的端口映射)
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource7);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource6);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);

    // 4. 配置外部中断 (EXTI)
    EXTI_InitTypeDef EXTI_InitStruct;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt; // 中断模式
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling; // 下降沿触发

    // 配置 EXTI Line 7, 6, 1
    EXTI_InitStruct.EXTI_Line = EXTI_Line7 | EXTI_Line6 | EXTI_Line1;
    EXTI_Init(&EXTI_InitStruct);

    // 配置 EXTI Line 0
    EXTI_InitStruct.EXTI_Line = EXTI_Line0;
    EXTI_Init(&EXTI_InitStruct);

    // 5. 配置 NVIC
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;

    // 配置 EXTI Line7 对应的中断
    NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_Init(&NVIC_InitStruct);

    // 配置 EXTI Line6 对应的中断
    NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_Init(&NVIC_InitStruct);

    // 配置 EXTI Line1 对应的中断
    NVIC_InitStruct.NVIC_IRQChannel = EXTI1_IRQn;
    NVIC_Init(&NVIC_InitStruct);

    // 配置 EXTI Line0 对应的中断
    NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_Init(&NVIC_InitStruct);
}












