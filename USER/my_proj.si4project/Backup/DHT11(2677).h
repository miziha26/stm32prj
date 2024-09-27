#ifndef DHT11_H
#define DHT11_H
	 
#include "stm32f10x.h"

// DHT11 数据引脚定义
#define DHT11_PIN GPIO_Pin_4  // 使用 PB4
#define DHT11_IO GPIOB        // GPIO 端口为 PB

// 定义 DHT11 引脚操作的宏
#define DHT11_DQ_OUT PAout(4) // 定义 DQ 输出引脚
#define DHT11_DQ_IN  PAin(4)  // 定义 DQ 输入引脚

// 函数声明
void DHT11_Rst(void);                 // 复位DHT11
u8 DHT11_Check(void);                 // 检测DHT11的存在
u8 DHT11_Read_Bit(void);              // 读取1位数据
u8 DHT11_Read_Byte(void);             // 读取1字节数据
u8 DHT11_Read_Data(u8 *temp, u8 *humi); // 读取温湿度数据
u8 DHT11_Init(void);                  // 初始化DHT11

#endif
