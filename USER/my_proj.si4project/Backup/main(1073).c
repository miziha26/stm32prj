#include "stm32f10x.h"
#include "led.h"
#include "oled.h"
#include "OLED_Data.h"
//#include "key.h"
#include "delay.h"
#include "SG90.h"
void test_led(void)
{
	Init_Led();
	LED_ALL_ON;
	while(1);
}

//void test_key(void)
//{
//	Init_Led();
//	Init_Key();
//	int BitVal1=0,BitVal2=0,BitVal3=0,BitVal4=0;
//	while(1)
//	{
//		if(Key_Status(KEY0) == KEY_DOWN)//触发按下
//		{
//			delay_ms(15);//跳过按下抖动
//			if(Key_Status(KEY0) == KEY_DOWN)//看是否被调戏了 是否真的按下
//			{
//				//稳定闭合期间
//				//翻转LED0
//				BitVal1 = (BitVal1+1) % 2;
//				GPIO_WriteBit(GPIOA, GPIO_Pin_11, (BitAction)BitVal1);
//				//当前的IO口输出状态在ODR中 就是只需要翻转当前的状态
//				while(Key_Status(KEY0) != KEY_UP);//等待弹起
//				delay_ms(15);//跳过松开抖动
//				while(Key_Status(KEY0) != KEY_UP);//稳定弹起
//			}
//		}
//		if(Key_Status(KEY1) == KEY_DOWN)
//		{
//			delay_ms(15);//跳过按下抖动
//			if(Key_Status(KEY1) == KEY_DOWN)//看是否被调戏了 是否真的按下
//			{
//				//稳定闭合期间
//				//翻转LED1
//				BitVal2 = (BitVal2+1) % 2;
//				GPIO_WriteBit(GPIOA, GPIO_Pin_12, (BitAction)BitVal2);
//				//当前的IO口输出状态在ODR中 就是只需要翻转当前的状态
//				while(Key_Status(KEY1) != KEY_UP);//等待弹起
//				delay_ms(15);//跳过松开抖动
//				while(Key_Status(KEY1) != KEY_UP);//稳定弹起
//			}

//		}
//		if(Key_Status(KEY2) == KEY_DOWN)
//		{
//			delay_ms(15);//跳过按下抖动
//			if(Key_Status(KEY2) == KEY_DOWN)//看是否被调戏了 是否真的按下
//			{
//				//稳定闭合期间
//				//翻转BEEP
//				BitVal3 = (BitVal3+1) % 2;
//				GPIO_WriteBit(GPIOA, GPIO_Pin_15, (BitAction)BitVal3);
//				//当前的IO口输出状态在ODR中 就是只需要翻转当前的状态
//				while(Key_Status(KEY2) != KEY_UP);//等待弹起
//				delay_ms(15);//跳过松开抖动
//				while(Key_Status(KEY2) != KEY_UP);//稳定弹起
//			}		
//		}
//		if(Key_Status(KEY3) == KEY_DOWN)
//		{
//			delay_ms(15);//跳过按下抖动
//			if(Key_Status(KEY3) == KEY_DOWN)//看是否被调戏了 是否真的按下
//			{
//				//稳定闭合期间
//				//翻转BEEP
//				BitVal4 = (BitVal4+1) % 2;
//				GPIO_WriteBit(GPIOB, GPIO_Pin_3, (BitAction)BitVal4);
//				//当前的IO口输出状态在ODR中 就是只需要翻转当前的状态
//				while(Key_Status(KEY3) != KEY_UP);//等待弹起
//				delay_ms(15);//跳过松开抖动
//				while(Key_Status(KEY3) != KEY_UP);//稳定弹起
//			}		
//		}
//	}	
//}

//void test_beep(void)
//{
//    while(1);
//}

void test_oled(void)
{
	// 初始化OLED显示屏
    OLED_Init();
    
    // 清空显示
    OLED_Clear();
    
    // 显示一些文本
		OLED_ShowChinese(0, 0, "你好世界");
    
    // 显示一个数字
    OLED_ShowNum(0, 16, 12345, 5, OLED_8X16);
    
    // 绘制一个矩形
    OLED_DrawRectangle(0, 32, 64, 16, OLED_UNFILLED);
    
    // 绘制一个圆
    OLED_DrawCircle(96, 40, 20, OLED_UNFILLED);
		//OLED_ShowImage(0, 0, 16, 16, Diode);

    
    // 更新显示以显示所有更改
    OLED_Update();
}

void testsg90(void)
{
	delay_ms(100);  //延时100ms，等电压稳定下来再初始化代码（可以防止OLED上电不亮等问题）
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	myServo_Init(1999,719);        //定时器2，产生PWM波，周期20ms，50HZ，控制舵机
	OLED_Init();          //OLED初始化
	OLED_Clear();         //清屏
	OLED_Update();       //更新显存到OLED(即刷新刚写入的数据,刷新屏幕)
	
	//OLED静态显示字符
	OLED_ShowString(0,0,"chenjiayou!",16);
	OLED_Update();             //更新显存到OLED(即刷新刚写入的数据,刷新屏幕)
	 
	while(1)
	{
		Servo1234_0du();   	//舵机转到0°
		Servo1234_180du(); 	//舵机转到180°
	}
}

int main()
{
/*全局环境参数*/
/*关闭j-tag*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOF, ENABLE);	 //使能相应接口的时钟，以及RCC_APB2Periph_AFIO
	//GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);//完全禁用SWD及JTAG 
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);	  //禁用JTAG

/*代码*/
	//test_led();
	test_oled();	
	//test_key();
	testsg90();
}
