#include "adc.h"
#include "delay.h"
#include "uasrt.h"  // 如果需要通过串口输出数据

int smoke_concentration;

void Adc_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    ADC_InitTypeDef   ADC_InitStruct;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);

    // 将PA4配置为模拟输入，用于连接MQ2传感器
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;                
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;  // 模拟输入模式
    GPIO_Init(GPIOA, &GPIO_InitStructure);             

    RCC_ADCCLKConfig(RCC_PCLK2_Div6);  // 设置ADC时钟为PCLK2/6

    ADC_DeInit(ADC1);
    
    ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStruct.ADC_NbrOfChannel = 1;
    ADC_InitStruct.ADC_ScanConvMode = DISABLE;
    
    ADC_Init(ADC1, &ADC_InitStruct);
    ADC_Cmd(ADC1, ENABLE);
    
    ADC_ResetCalibration(ADC1);    
    while(ADC_GetResetCalibrationStatus(ADC1));  // 等待校准复位结束
    
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));  // 等待校准结束
}

u16 Get_Adc(u8 ch)  // 获取ADC数据
{
    ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
    return ADC_GetConversionValue(ADC1);
}

u16 Get_Adc_Average(u8 ch, u8 times)
{
    u32 temp_val = 0;
    u8 t;

    for (t = 0; t < times; t++)
    {
        temp_val += Get_Adc(ch);
        delay_ms(1);
    }

    // 计算ADC平均值
    u16 avg_adc_value = temp_val / times;

    return avg_adc_value;
}

