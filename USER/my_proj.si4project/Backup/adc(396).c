//#include "adc.h"
//#include "delay.h"

//int shidu1;
//void Adc_Init(void)
//{
//    GPIO_InitTypeDef  GPIO_InitStructure;
//    ADC_InitTypeDef   ADC_InitStruct;
//    
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);

//    /* 将PA4设置为模拟输入 */
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;                
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;       // 模拟输入模式
//    GPIO_Init(GPIOA, &GPIO_InitStructure);             

//    /* 设置ADC时钟分频 */
//    RCC_ADCCLKConfig(RCC_PCLK2_Div6);  // ADC时钟为PCLK2/6
//    
//    /* 复位ADC1 */
//    ADC_DeInit(ADC1);
//    
//    /* 初始化ADC */
//    ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;
//    ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
//    ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
//    ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
//    ADC_InitStruct.ADC_NbrOfChannel = 1;
//    ADC_InitStruct.ADC_ScanConvMode = DISABLE;
//    
//    ADC_Init(ADC1, &ADC_InitStruct);
//    
//    /* 启用ADC1 */
//    ADC_Cmd(ADC1, ENABLE);
//    
//    /* 复位校准 */
//    ADC_ResetCalibration(ADC1);    
//    while(ADC_GetResetCalibrationStatus(ADC1));  // 等待校准复位结束
//    
//    /* 开始校准 */
//    ADC_StartCalibration(ADC1);
//    while(ADC_GetCalibrationStatus(ADC1));  // 等待校准结束
//}

//u16 Get_Adc(u8 ch)  // 获取ADC数据
//{
//    ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5);
//    
//    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
//    
//    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
//    
//    return ADC_GetConversionValue(ADC1);
//}

//u16 Get_Adc_Average(u8 ch, u8 times)  // 获取土壤湿度数据并返回给主函数
//{
//    u32 temp_val = 0;
//    float temp_avrg = 0;
//    u8 t;
//    
//    for(t = 0; t < times; t++)
//    {
//        temp_val += Get_Adc(ch);
//        delay_ms(1);
//    }
//    
//    temp_avrg = temp_val / times;
//    shidu1 = (4092 - temp_avrg) / 3292 * 100;  // 计算湿度值
//    return shidu1;
//}
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
    
    for(t = 0; t < times; t++)
    {
        temp_val += Get_Adc(ch);
        delay_ms(1);
    }
    
    return temp_val / times;  // 返回平均ADC值
}

float Calculate_SmokeConcentration(u16 adc_value)
{
    // 将ADC值转换为电压 (假设ADC最大值为4095，参考电压为3.3V)
    float voltage = (adc_value / 4095.0) * 3.3;
    
    // 根据实验曲线或者公式计算烟雾浓度
    // 这里需要使用MQ2传感器的标定数据，可以根据具体情况进行调整
    float concentration = (voltage - 0.1) * 1000;  // 示例公式，请根据实际情况修改
    
    return concentration;
}

void Test_SmokeConcentration(void)
{
    u16 adc_value;
    float smoke_concentration;
    
    // 初始化ADC
    Adc_Init();
    
    while(1)
    {
        // 获取MQ2传感器的ADC值
        adc_value = Get_Adc_Average(ADC_Channel_4, 10);  // PA4
        // 计算烟雾浓度
        smoke_concentration = Calculate_SmokeConcentration(adc_value);
        
        // 打印烟雾浓度到串口
        printf("Smoke Concentration: %.2f ppm\r\n", smoke_concentration);
        
        delay_ms(1000);  // 延时1秒后再次读取
    }
}
