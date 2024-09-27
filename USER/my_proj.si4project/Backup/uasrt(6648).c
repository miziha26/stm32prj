#include "stm32f10x.h"
#include <stdio.h>
#include "led.h"

/*
	Init_UART1:初始化串口1 并且设置它的波特率
	@USART_BaudRate:波特率
	返回值为空
*/
void Init_UART1(uint32_t USART_BaudRate)
{

	//1.使能时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//PA9、PA10
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);//USART1
	//2.配置GPIO
	GPIO_InitTypeDef GPIO_InitStruct={0};
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;//模式复用、推挽输出
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	//3.配置USART控制器
	USART_InitTypeDef USART_InitStruct={0};
	USART_InitStruct.USART_BaudRate = USART_BaudRate;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无流控
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//全双工
	USART_InitStruct.USART_Parity = USART_Parity_No;//无校验
	USART_InitStruct.USART_StopBits = USART_StopBits_1;//1位停止位
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;//8位数据位
	USART_Init(USART1, &USART_InitStruct);
	//4.配置中断
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	NVIC_InitTypeDef NVIC_InitStruct={0};
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStruct);
	//5.使能串口
	USART_Cmd(USART1, ENABLE);
}


/*
	UART1_Send_Datas:串口发送数据
	@sendbuf:存放数据的地方
	@n:发送n个字节
	返回值为空 
*/
void UART1_Send_Datas(uint8_t* sendbuf,uint16_t n)

{
	int i;
	for(i=0;i<n;i++)
	{
		USART_SendData(USART1,sendbuf[i]);//发送一个字节
    	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)!=SET);//等待发送完成
	}
}

/*接收中断函数*/
//uint16_t USART1_DATA;
//void USART1_IRQHandler(void)
//{
//    if(USART_GetITStatus(USART1,USART_IT_RXNE)==SET)
//    {
//        //读数据
//        USART1_DATA = USART_ReceiveData(USART1);
//       	USART_ClearITPendingBit(USART1, USART_IT_RXNE);
//        if(USART1_DATA == 'A')
//        {
//        	LED_ALL_ON;
//        }
//        else if(USART1_DATA == 'B')
//        {
//        	LED_ALL_OFF;
//        }
//    }
//}

uint8_t usart1_buffer[3];
uint8_t buffer_index = 0;
void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
    {        
        // 判断缓冲区是否已满
        while(buffer_index < 3-1)
        {
			uint16_t received_char = USART_ReceiveData(USART1);  // 接收单个字符
            usart1_buffer[buffer_index++] = received_char;  // 存入缓冲区            
        }
		usart1_buffer[buffer_index] = '\0';  // 添加字符串结束符
        buffer_index = 0;  // 重置缓冲区索引

        // 清除中断标志
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}


int fputc(int c,FILE* stream)
{
	USART_SendData(USART1,c&0Xff);//发送一个字节
    while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)!=SET);
    return 0;
}



