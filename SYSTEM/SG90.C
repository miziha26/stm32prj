#include "stm32f10x.h"
#include "SG90.h"
#include "delay.h"

/*
*** TIM4 配置定时器4 PWM控制SG90舵机 ***
 
*** PWM频率：50Hz ***
 
*** 接口：PB8->舵机1   PB9->舵机2   ***
 
*/
void myServo_Init(u16 arr, u16 psc)
{  
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
	
    // 使能定时器4时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    // 使能GPIOB外设时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
    // 初始化输出TIM4两个通道PWM脉冲波形的引脚 (CH3->PB8, CH4->PB9)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  // 复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);  // 初始化GPIO
	
    // 初始化TIM4
    TIM_TimeBaseStructure.TIM_Period = arr;  // 设置自动重装载寄存器周期的值
    TIM_TimeBaseStructure.TIM_Prescaler = psc;  // 设置预分频值
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;  // 设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  // TIM向上计数模式
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);  // 初始化TIM4的时间基数单位
	
    // 初始化TIM4 Channel PWM模式
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;  // 选择定时器模式:TIM脉冲宽度调制模式1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  // 比较输出使能
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  // 输出极性:TIM输出比较极性高
	
    // 初始化TIM4 CH3 (PB8) 和 CH4 (PB9)
    TIM_OC3Init(TIM4, &TIM_OCInitStructure);  // 初始化外设TIM4 OC3
    TIM_OC4Init(TIM4, &TIM_OCInitStructure);  // 初始化外设TIM4 OC4
  
    TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);  // 使能TIM4在CCR3上的预装载寄存器
    TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);  // 使能TIM4在CCR4上的预装载寄存器
	
    TIM_Cmd(TIM4, ENABLE);  // 使能TIM4
}

/*
*** TIM4 两个通道输出PWM 控制SG90舵机 舵机回零 ***
 
*** PWM频率：50Hz 高电平0.5ms ***
 
*** 接口：PB8->舵机1   PB9->舵机2 ***
*/
void Servo12_0du(void)
{
    TIM_SetCompare3(TIM4, 50);  // 舵机1 0.5ms
    TIM_SetCompare4(TIM4, 50);  // 舵机2 0.5ms
	
    delay_ms(500);
}

/*
*** TIM4 两个通道输出PWM 控制SG90舵机 舵机转到最大角度 ***
 
*** PWM频率：50Hz 高电平2.5ms ***
 
*** 接口：PB8->舵机1   PB9->舵机2 ***
*/
void Servo12_180du(void)
{
    TIM_SetCompare3(TIM4, 250);  // 舵机1 2.5ms
    TIM_SetCompare4(TIM4, 250);  // 舵机2 2.5ms
	
    delay_ms(500);
}

//将舵机1视为窗户，舵机2视为门，0度为关闭，180度为打开
void openwin(void)
{
	TIM_SetCompare3(TIM4, 250);
}
void closewin(void)
{
	TIM_SetCompare3(TIM4, 50);
}
void opendoor(void)
{
	TIM_SetCompare4(TIM4, 250);
}
void closedoor(void)
{
	TIM_SetCompare4(TIM4, 50);
}



