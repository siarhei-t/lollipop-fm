/**
 * @file buzzer.cpp
 *
 * @brief
 *
 */

#include "buzzer.hpp"
#include <stm32c011xx.h>

namespace bz
{

Buzzer& Buzzer::instance()
{
    static Buzzer bz;
    return bz;
}

Buzzer::Buzzer()
{

    RCC->IOPENR |= RCC_IOPENR_GPIOBEN;
    RCC->APBENR1 |= RCC_APBENR1_TIM3EN;

    // PB7 AF3 TIM3_CH2
    GPIOB->MODER &= ~GPIO_MODER_MODE7;
    GPIOB->MODER |= GPIO_MODER_MODE7_1;
    GPIOB->AFR[0] &= ~GPIO_AFRL_AFSEL7_Msk;
    GPIOB->AFR[0] |= (3U << GPIO_AFRL_AFSEL7_Pos);
    // TIM3 Channel 4 PWM mode 1
    TIM3->CCMR2 &= ~(TIM_CCMR2_OC4M_Msk | TIM_CCMR2_OC4PE);
    TIM3->CCMR2 |= (6U << TIM_CCMR2_OC4M_Pos) | TIM_CCMR2_OC4PE;
    TIM3->PSC = 0;

    uint32_t period = SystemCoreClock / default_frequency_hz - 1;
    TIM3->ARR = period;
    TIM3->CCR4 = (period + 1) / 2; // 50%
    TIM3->CR1 |= TIM_CR1_ARPE;
    TIM3->EGR = TIM_EGR_UG;
}

void Buzzer::start()
{
    TIM3->CCER |= TIM_CCER_CC4E;
    TIM3->CR1 |= TIM_CR1_CEN;
}

void Buzzer::stop()
{
    TIM3->CCER &= ~TIM_CCER_CC4E;
    TIM3->CR1 &= ~TIM_CR1_CEN;
}

void Buzzer::setFrequency(const std::uint16_t frequency_hz)
{
    uint32_t timer_clk = SystemCoreClock;
    uint32_t period = timer_clk / frequency_hz - 1;

    TIM3->ARR = period;
    TIM3->CCR2 = (period + 1) / 2;
}

void Buzzer::buzzerTask(void* pvParameters)
{
    (void)(pvParameters);

    for (;;)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void Buzzer::init() { task_handle = xTaskCreateStatic(&Buzzer::buzzerTask, "buzzer control", stack_size, nullptr, task_priority, stack, &task_buffer); }

} // namespace bz
