#include "tim3.h"

uint8_t timeout_flag = 0;  // 超时标志变量

// TIM3 定时器初始化函数
void TIM3_Init(uint16_t ms) {
    // 1. 开启 TIM3 和 GPIOB 的时钟
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;  // 开启 TIM3 时钟
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;  // 开启 GPIOB 时钟

    // 2. 配置 GPIOB1 为复用功能模式，作为 TIM3_CH4 输出
    GPIOB->CRL &= ~GPIO_CRL_MODE1;       // 清除 MODE1
    GPIOB->CRL |= GPIO_CRL_MODE1_1;      // 配置 PB1 为输出模式，最大 2 MHz
    GPIOB->CRL &= ~GPIO_CRL_CNF1;        // 清除 CNF1
    GPIOB->CRL |= GPIO_CRL_CNF1_1;       // 配置为复用推挽输出

    // 3. 配置 TIM3 的计时参数
    TIM3->PSC = 7200 - 1;    // 设置预分频器：72 MHz / 7200 = 10 kHz 计时频率
    TIM3->ARR = ms - 1;   // 设置自动重装载值：10 kHz / 10000 = 1 秒定时

    // 4. 清零计数器
    TIM3->CNT = 0;

    // 5. 启用更新中断
    TIM3->DIER |= TIM_DIER_UIE;

    // 6. 启动定时器
    TIM3->CR1 |= TIM_CR1_CEN;

    // 7. 配置 NVIC，开启 TIM3 中断
    NVIC_EnableIRQ(TIM3_IRQn);
    NVIC_SetPriority(TIM3_IRQn, 1);  // 设置优先级
}

// TIM3 中断服务程序
void TIM3_IRQHandler(void) {
    if (TIM3->SR & TIM_SR_UIF) {  // 检查更新中断标志
        TIM3->SR &= ~TIM_SR_UIF;  // 清除更新中断标志
        timeout_flag = 1;         // 设置超时标志
        TIM3->CR1 &= ~TIM_CR1_CEN;  // 停止定时器
    }
}

