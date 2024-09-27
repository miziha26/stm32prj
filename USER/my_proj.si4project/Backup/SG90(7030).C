#include "stm32f10x.h"
#include "SG90.h"
#include "delay.h"


/*
*** TIM2  配置定时器2PWM控制SG90舵机 ***
 
*** PWM频率：50Hz ***
 
*** 接口：PA0->舵机1   PA1->舵机2   PA2->舵机3   PA3->舵机4 ***
 
*** 作者：陈加油嗯 ***
 
***关注微信公众号学习更多单片机知识：微信搜索“陈加油嗯” ***
 
*/
void myServo_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//使能定时器2时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //使能GPIO外设；————假如要AFIO复            用功能模块时钟则要加上（  | RCC_APB2Periph_AFIO）
	
	//GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); //Timer3部分重映射  TIM3_CH2->PB5    
 
   //初始化输出TIM2四个通道PWM脉冲波形的引脚(CH1->PA0 CH2->PA1 CH3->PA2 CH4->PA3)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3; //CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO
 
   //初始化TIM2
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	//初始化TIM2 Channe PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式1
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM2 OC1
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM2 OC2
	TIM_OC3Init(TIM2, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM2 OC3
	TIM_OC4Init(TIM2, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM2 OC4
  
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);  //使能TIM2在CCR1上的预装载寄存器
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);  //使能TIM2在CCR1上的预装载寄存器
	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);  //使能TIM2在CCR1上的预装载寄存器
	TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);  //使能TIM2在CCR1上的预装载寄存器
	
	TIM_Cmd(TIM2, ENABLE);  //使能TIM 2
}



/*
*** TIM2 四个通道输出PWM 控制SG90舵机 舵机回零 ***
 
*** PWM频率：50Hz   高电平0.5ms ***
 
*** 接口：PA0->舵机1   PA1->舵机2   PA2->舵机3   PA3->舵机4 ***
 
*** 作者：陈加油嗯 ***
 
***关注微信公众号学习更多单片机知识：微信搜索“陈加油嗯” ***
 
*/
void Servo1234_0du(void)
{
	TIM_SetCompare1(TIM2,50);  //舵机1 0.5ms
	TIM_SetCompare2(TIM2,50);  //舵机2 0.5ms
	TIM_SetCompare3(TIM2,50);  //舵机3 0.5ms
	TIM_SetCompare4(TIM2,50);  //舵机4 0.5ms
	
	delay_ms(500);
}
 
/*
*** TIM2 四个通道输出PWM 控制SG90舵机 舵机转到最大角度 ***
 
*** PWM频率：50Hz   高电平2.5ms ***
 
*** 接口：PA0->舵机1   PA1->舵机2   PA2->舵机3   PA3->舵机4 ***
 
*** 作者：陈加油嗯 ***
 
***关注微信公众号学习更多单片机知识：微信搜索“陈加油嗯” ***
 
*/
void Servo1234_180du(void)
{
	TIM_SetCompare1(TIM2,250);  //舵机1 2.5ms
	TIM_SetCompare2(TIM2,250);  //舵机2 2.5ms
	TIM_SetCompare3(TIM2,250);  //舵机3 2.75ms
	TIM_SetCompare4(TIM2,250);  //舵机4 2.75ms
	
	delay_ms(500);
}






