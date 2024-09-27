#include "stm32f10x.h"
#include "led.h"
#include "oled.h"
#include "OLED_Data.h"
#include "key.h"
#include "delay.h"
#include "beep.h"
#include "SG90.h"
#include "uasrt.h"
#include <stdio.h>
void test_led(void)
{
	Init_Led();
	LED_ALL_ON;
	while(1);
}

void test_key(void)
{
	Init_Led();
	Key_Init();
	int BitVal1=0,BitVal2=0,BitVal3=0,BitVal4=0;
	while(1)
	{
		if(Key_Status(KEY0) == KEY_DOWN)//触发按下
		{
			delay_ms(15);//跳过按下抖动
			if(Key_Status(KEY0) == KEY_DOWN)//看是否被调戏了 是否真的按下
			{
				//稳定闭合期间
				//翻转LED1
				BitVal1 = (BitVal1+1) % 2;
				GPIO_WriteBit(GPIOA, GPIO_Pin_11, (BitAction)BitVal1);
				//当前的IO口输出状态在ODR中 就是只需要翻转当前的状态
				while(Key_Status(KEY0) != KEY_UP);//等待弹起
				delay_ms(15);//跳过松开抖动
				while(Key_Status(KEY0) != KEY_UP);//稳定弹起
			}
		}
		if(Key_Status(KEY1) == KEY_DOWN)
		{
			delay_ms(15);//跳过按下抖动
			if(Key_Status(KEY1) == KEY_DOWN)//看是否被调戏了 是否真的按下
			{
				//稳定闭合期间
				//翻转LED2
				BitVal2 = (BitVal2+1) % 2;
				GPIO_WriteBit(GPIOA, GPIO_Pin_12, (BitAction)BitVal2);
				//当前的IO口输出状态在ODR中 就是只需要翻转当前的状态
				while(Key_Status(KEY1) != KEY_UP);//等待弹起
				delay_ms(15);//跳过松开抖动
				while(Key_Status(KEY1) != KEY_UP);//稳定弹起
			}

		}
		if(Key_Status(KEY2) == KEY_DOWN)
		{
			delay_ms(15);//跳过按下抖动
			if(Key_Status(KEY2) == KEY_DOWN)//看是否被调戏了 是否真的按下
			{
				//稳定闭合期间
				//翻转led3
				BitVal3 = (BitVal3+1) % 2;
				GPIO_WriteBit(GPIOA, GPIO_Pin_15, (BitAction)BitVal3);
				//当前的IO口输出状态在ODR中 就是只需要翻转当前的状态
				while(Key_Status(KEY2) != KEY_UP);//等待弹起
				delay_ms(15);//跳过松开抖动
				while(Key_Status(KEY2) != KEY_UP);//稳定弹起
			}		
		}
		if(Key_Status(KEY3) == KEY_DOWN)
		{
			delay_ms(15);//跳过按下抖动
			if(Key_Status(KEY3) == KEY_DOWN)//看是否被调戏了 是否真的按下
			{
				//稳定闭合期间
				//翻转led4
				BitVal4 = (BitVal4+1) % 2;
				GPIO_WriteBit(GPIOB, GPIO_Pin_3, (BitAction)BitVal4);
				//当前的IO口输出状态在ODR中 就是只需要翻转当前的状态
				while(Key_Status(KEY3) != KEY_UP);//等待弹起
				delay_ms(15);//跳过松开抖动
				while(Key_Status(KEY3) != KEY_UP);//稳定弹起
			}		
		}
	}	
}

void test_beep(void)
{
	Beep_Init();
	Key_Init();
	while(1)
	{
		if(Key_Status(KEY0) == KEY_DOWN)//触发按下
		{
			delay_ms(15);//跳过按下抖动
			if(Key_Status(KEY0) == KEY_DOWN)//看是否被调戏了 是否真的按下
			{
				//稳定闭合期间
				//翻转BEEP
				Beep_Toggle();
				//当前的IO口输出状态在ODR中 就是只需要翻转当前的状态
				while(Key_Status(KEY0) != KEY_UP);//等待弹起
				delay_ms(15);//跳过松开抖动
				while(Key_Status(KEY0) != KEY_UP);//稳定弹起
			}
		}	
	}	
}

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
    delay_ms(100);  // 延时100ms，等电压稳定下来再初始化代码（可以防止OLED上电不亮等问题）
	
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    myServo_Init(1999,719);  // 初始化定时器4，产生PWM波，周期20ms，50HZ，控制舵机
    OLED_Init();  // OLED初始化
    OLED_Clear();  // 清屏
	
    // OLED静态显示字符
    OLED_ShowString(20, 20, "ABCDE", OLED_8X16);
    OLED_ShowChinese(0, 0, "你好世界");
    OLED_Update();  // 更新显存到OLED(即刷新刚写入的数据,刷新屏幕)
	 
    while(1)
    {	
		OLED_Clear();
        OLED_ShowChinese(0, 0, "开门啦");
		OLED_Update();
        opendoor();  // 舵机2转到180°
        delay_ms(1000);  // 延时1000ms，确保动作完成
        
        OLED_Clear();
		OLED_ShowChinese(0, 0, "关门啦");
		OLED_Update();
        closedoor();  // 舵机2转到0°
        delay_ms(1000);  // 延时1000ms，确保动作完成
    }
}

// 全局变量，用于存储接收到的USART1数据
extern uint16_t USART1_DATA;
void test_usart(void)
{
    // 1. 初始化OLED屏幕
    OLED_Init();  // OLED初始化
    OLED_Clear();  // 清屏  

    // 2. 初始化串口1，波特率设置为9600
    Init_UART1(9600);

    // 3. 在OLED屏幕上显示初始化信息
    OLED_ShowString(0, 0, "UART1 Test", OLED_8X16);  // 在OLED上显示 "UART1 Test"
    OLED_ShowString(0, 16, "Received:", OLED_8X16);  // 显示 "Received:" 文本用于接收数据提示
    OLED_Update();

    // 4. 发送一段测试数据
    while(1)
		{
			uint8_t send_data[] = "Hello UART1!";
			UART1_Send_Datas(send_data, sizeof(send_data) - 1);  // 发送字符串
			
			delay_ms(5);
		}

//    while(1)
//    {
//        // 串口接收的处理已经在中断服务函数USART1_IRQHandler中完成
//        // 可以在OLED屏幕上显示接收到的数据
//        if (USART1_DATA != 0)  // 假设USART1_DATA是接收的字符变量
//        {
//            char buffer[16];
//            sprintf(buffer, "Data: %c", USART1_DATA);  // 将接收的数据格式化成字符串
//            OLED_ShowString(0, 32, buffer, OLED_8X16);  // 在OLED上显示接收到的数据
//            OLED_Update();
//            USART1_DATA = 0;  // 清空数据，防止重复显示
//        }
//    }
}


int main()
{
/*全局环境参数*/
/*关闭j-tag*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOF, ENABLE);	 //使能相应接口的时钟，以及RCC_APB2Periph_AFIO
	//GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);//完全禁用SWD及JTAG 
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);	  //禁用JTAG
	/*中断的抢占优先级和子优先级配置*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//2:2
	NVIC_SetPriority(SysTick_IRQn,0);
/*代码*/
	//test_led();
	//test_oled();	
	//test_key();
	//testsg90();
	//test_beep();
	test_usart();
}
