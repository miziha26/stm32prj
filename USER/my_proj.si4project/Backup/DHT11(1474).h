#ifndef __DHT11_H
#define __DHT11_H
#include "stm32f10x.h"

 
 
/* 设置GPIO脚，默认为PB11 */
#define DHT11_IO 		            GPIOB
#define DHT11_PIN		            GPIO_Pin_7
#define DHT11_APB2PeriphRCC     RCC_APB2Periph_GPIOB
/* 初始化函数，如果DHT11存在响应则返回1，否则0 */
u8 DHT11_Init(void);
/* 从DHT11读取数据，没有小数部分 */
 u8 DHT11_Read_Data(u8 *temp,u8 *humi);
 
#endif
 
//#ifndef __DHT11_H
//#define __DHT11_H	 
//#include "stm32f10x.h"

//// 定义 DHT11 数据引脚
//#define DHT11_PIN        GPIO_Pin_4  // 使用 PB4 作为数据引脚
//#define DHT11_IO         GPIOB       // GPIO 端口为 PB

//// 宏定义控制 DHT11 数据引脚的输出
//#define DHT11_DQ_OUT(val)  (val ? GPIO_SetBits(GPIOB, GPIO_Pin_4) : GPIO_ResetBits(GPIOB, GPIO_Pin_4))

//// 宏定义读取 DHT11 数据引脚的输入
//#define DHT11_DQ_IN        GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4)


//// DHT11 函数声明
//void DHT11_Rst(void);            // 复位 DHT11
//u8 DHT11_Check(void);            // 检测 DHT11 的存在
//u8 DHT11_Read_Bit(void);         // 读取一个位
//u8 DHT11_Read_Byte(void);        // 读取一个字节
//u8 DHT11_Read_Data(u8 *temp, u8 *humi);  // 读取温湿度数据
//u8 DHT11_Init(void);             // 初始化 DHT11
//void DHT11_IO_IN(void);
//void DHT11_IO_OUT(void);

//#endif
