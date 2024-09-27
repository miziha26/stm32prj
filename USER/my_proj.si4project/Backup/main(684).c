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
#include "DEBUG.h"
#include "W25Q128.h"
#include "HC05.h"
#include "adc.h"
#include "DHT11.h"
#include "string.h"
#include "tim3.h"

extern uint8_t timeout_flag;

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

extern uint8_t usart1_buffer[128];
extern uint8_t data_received_flag;
void test_usart(void)
{
    // 1. 初始化OLED屏幕
    Init_Led();
    OLED_Init();  // OLED初始化
    OLED_Clear();  // 清屏  

    // 2. 初始化串口1，波特率设置为9600
    Init_UART1(9600);

    // 3. 在OLED屏幕上显示初始化信息
    OLED_ShowString(0, 0, "UART1 Test", OLED_8X16);  // 在OLED上显示 "UART1 Test"
    OLED_ShowString(0, 16, "Received:", OLED_8X16);  // 显示 "Received:" 文本用于接收数据提示
    OLED_Update();

    // 4. 发送一段测试数据
    
	uint8_t send_data[] = "Hello UART1!";
	UART1_Send_Datas(send_data, sizeof(send_data) - 1);  // 发送字符串


	
    while(1)
    {
		// 检查是否有接收到完整的字符串
        if (data_received_flag)
        {	
			OLED_ClearArea(0, 32, 128, 16);
            OLED_ShowString(0, 32, (char *)usart1_buffer, OLED_8X16);  // 显示接收到的字符串
            OLED_Update();  // 刷新 OLED 显示

            data_received_flag = 0;  // 清除标志位，准备接收新的数据
        }
    }

}

#define TEST_ADDRESS  0x000000  // 需要测试的地址
#define TEST_DATA_LEN 16         // 测试的数据长度
u8 test_write_data[TEST_DATA_LEN] = {0xAA, 0x55, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC,                                    0xDE, 0xF0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
u8 test_read_data[TEST_DATA_LEN] = {0};  // 用于存放读取到的数据
void W25Q128_Test(void)
{	
	delay_ms(2000);
	Init_UART1(9600);
	W25Q128_SPI_Init();
    // 测试数据
    u8 write_data[256];
    u8 read_data[256];
    int i;
    u32 test_addr = 0x00010000;  // 测试地址 (注意不要选在重要的数据区域)

    // 初始化测试数据
    for (i = 0; i < 256; i++) {
        write_data[i] = i;  // 填充一些递增的数据用于测试
    }

    // Step 1: 写入测试数据到W25Q128
    printf("Writing data to W25Q128...\n");
    W25Q128_Write_Bytes(test_addr, write_data, 256);

    while(1)
    {
		// Step 2: 从W25Q128读取数据
    	printf("Reading data from W25Q128...\n");
    	W25Q128_Read_Bytes(test_addr, read_data, 256);
    

	    // Step 3: 验证数据
	    printf("Verifying data...\n");
	    for (i = 0; i < 256; i++) {
	        if (write_data[i] != read_data[i]) {
	            printf("Data mismatch at index %d: wrote %02X, read %02X\n", i, write_data[i], read_data[i]);
	            return;  // 终止测试，读写失败
	        }
		// 数据验证成功
    	printf("W25Q128 read/write test passed!\n");
    }
	}
    
}

void testhc05(void)
{
	Init_Led();
	Beep_Init();
	myServo_Init(1999,719);
	OLED_Init();
	OLED_Clear();
	USART2_Init();
	u8 Direction;
 
	while(1)
	{
		
		if (USART2_GetRxFlag() == 1)		//判断是否收到数据
		{
			Direction = USART2_GetRxData();		//读取数据
			if(Direction == 'A')
			{
				LED_ALL_ON;
				BEEP_ON;
				openwin();
			}
			if(Direction == 'B')
			{
				LED_ALL_OFF;
				BEEP_OFF;
				closewin();
			}
			OLED_ShowChar(0,0,Direction,OLED_8X16);
			OLED_Update();
			USART2_SendByte(Direction);		//将数据回传到电脑
		}
		
	}
}


void Test_SmokeConcentration(void)
{
	// 初始化ADC
    Adc_Init();
    Init_UART1(9600);  // 初始化 UART
    OLED_Init();  // 初始化 OLED
    OLED_Clear();  // 清屏
    
    u16 avg_adc_value;
    float smoke_concentration;
    char buffer[20];  // 用于存储格式化字符串

    while(1)
    {
        // 获取指定通道的平均ADC值，这里是PA4 
        avg_adc_value = Get_Adc_Average(4, 10);  // 获取10次采样的平均值

        // 计算烟雾浓度 
        smoke_concentration = 0.000806*avg_adc_value/(10/14.7)*20;

        // 串口打印结果
        printf("Smoke Value: %.2f ppm\n", smoke_concentration);

        // 将结果显示到 OLED
        OLED_Clear();  // 清除上一轮显示内容

        // 格式化烟雾浓度值
        sprintf(buffer, "Smoke: %.2f ppm", smoke_concentration);
		OLED_ShowString(0, 16, buffer,OLED_8X16); 
		OLED_Update();
        // 延时 1 秒
        delay_ms(1000);
    }
}

//void testdht11(void)
//{		
//    char dis1[16];
//    u8 tem1 = 0;
//    u8 hum1 = 0;
//    Init_UART1(9600);
//    if (!DHT11_Init()) {
//        printf("Error! DHT11 has no respond...\n");
//        return;  // 如果 DHT11 初始化失败，直接退出
//    }
//		while(1)
//    {
//			printf("timeout_flag = %d\n", timeout_flag);
//    }
//    printf("\r\nDHT11 has respond");
//    delay_ms(10);
//    // 初始化 TIM3，设置为 3 秒超时
//    TIM3_Init(3000);  // 初始化 TIM3，并设置为 3 秒的时间（单位 ms）
//    while (!timeout_flag)  // 当定时器没到 3 秒时执行循环
//    {
//        if (DHT11_Read_Data(&tem1, &hum1)) 
//		{
//            printf("\r\ntemp: %d, humi: %d", tem1, hum1);
//            sprintf(dis1, "TEMP%d HUM%d", tem1, hum1);
//            OLED_ShowString(0, 0, dis1, OLED_8X16);
//			OLED_Update();
//        } 
//		else 
//		{
//            printf("\r\nError! DHT11 has no respond...");
//        }
//        // 延迟1秒
//        delay_ms(1000);
//    }
//}

void testdht11(void)
{
	u8 temperature = 0;
    u8 humidity = 0;
    u8 result = 0;

	Init_UART1(9600);
    // 初始化 DHT11
    result = DHT11_Init();
    
    while(1)
    {
		if (result == 0) // 检测到 DHT11 传感器
	    {
	        printf("DHT11 sensor detected.\r\n");
	        
	        // 读取温湿度数据
	        result = DHT11_Read_Data(&temperature, &humidity);
	        if (result == 0) // 读取数据成功
	        {
	            printf("Temperature: %d°C\r\n", temperature);
	            printf("Humidity: %d%%\r\n", humidity);
	        }
	        else
	        {
	            printf("Failed to read data from DHT11 sensor.\r\n");
	        }
	    }
	    else
	    {
	        printf("DHT11 sensor not detected.\r\n");
	    }
	}
}


void Init(void)
{
	Init_Led();
	Key_Init();
	Beep_Init();
	OLED_Init();
    OLED_Clear();
	TIM3_Init(3000);
	myServo_Init(1999,719);
	Init_UART1(9600);
	W25Q128_SPI_Init();
	USART2_Init();
	Adc_Init();
	if(!DHT11_Init()) printf(" Error! T DHT11 HAS NO RESPOND...\n");
}


void run()
{
	u8 Direction; 
	while(1)
	{
		if (USART2_GetRxFlag() == 1)  // 判断是否收到数据
		{
			Direction = USART2_GetRxData();  // 读取数据
			switch (Direction)
			{
				case 'a': // 开卧室灯 LED1
				{
					LED1_ON;
					break;  
				}
				case 'b': // 关卧室灯
				{
					LED1_OFF;
					break;
				}
				case 'c': // 开门舵机 2
				{
					opendoor();
					break;
				}
				case 'd': // 开客厅灯 LED2, LED3
				{
					LED2_ON;
					LED3_ON;
					break;
				}
				case 'e': // 关客厅灯
				{
					LED2_OFF;
					LED3_OFF;
					break;
				}
				case 'f': // 关门
				{
					closedoor();
					break;
				}
				case 'g': // 开窗舵机 1
				{
					openwin();
					break;
				}
				case 'h': // 关窗
				{
					closewin();
					break;
				}
				case 'i': // 测温湿度
				{
					testdht11();
					break;
				}
				case 'j': // 测烟雾浓度
				{
					Test_SmokeConcentration();
					break;
				}
				case 'k': // 报警
				{
					BEEP_ON;
					LED_ALL_ON;
					opendoor();
					openwin();
					break;
				}
				case 'l': // 解除警报
				{
					BEEP_OFF;
					break;
				}
				case 'm': // 开全部灯
				{
					LED_ALL_ON;
					break;
				}
				case 'n': // 关全部灯
				{
					LED_ALL_OFF;
					break;
				}
				// 其他命令留作扩展
				// case 'o': 
				// {
				//    // 扩展功能
				//    break;
				// }
				default:
				{
					// 如果没有匹配的命令，可以在这里处理
					break;
				}
			}
			OLED_ShowChar(0, 0, Direction, OLED_8X16);  // 显示接收的数据
			OLED_Update();
			USART2_SendByte(Direction);  // 将数据回传到电脑
		}
	}
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
	Init();
	//test_led();
	//test_oled();	
	//test_key();
	//testsg90();
	//test_beep();
	//test_usart();
	//W25Q128_Test();
	//testhc05();
	//Test_SmokeConcentration();
	testdht11();
	//run();
}
