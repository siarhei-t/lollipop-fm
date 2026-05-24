/**
 * @file fm.cpp
 *
 * @brief
 *
 */

#include "fm.hpp"
#include "config.hpp"
#include <stm32c011xx.h>

namespace fm
{

// top value for TIM ARR register
constexpr std::uint32_t expected_core_clock_hz = 8000000;

FrequencyMeter& FrequencyMeter::instance()
{
    static FrequencyMeter fm;
    return fm;
}

FrequencyMeter::FrequencyMeter()
{
    RCC->APBENR2 |= RCC_APBENR2_TIM1EN;
    RCC->APBENR2 |= RCC_APBENR2_TIM17EN;
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
    // LOLLIPOP-FM rev. 1.0 board
    // PA12 AF2 TIM1_ETR
    GPIOA->MODER &= ~GPIO_MODER_MODE12;
    GPIOA->MODER |= GPIO_MODER_MODE12_1;
    GPIOA->AFR[1] &= ~GPIO_AFRH_AFSEL12;
    GPIOA->AFR[1] |= (2U << GPIO_AFRH_AFSEL12_Pos);
    // TIM1 External trigger input
    TIM1->PSC = 0;
    TIM1->SMCR &= ~TIM_SMCR_ETF;
    TIM1->SMCR &= ~TIM_SMCR_ETPS;
    TIM1->SMCR &= ~TIM_SMCR_ETP;
    TIM1->SMCR |= TIM_SMCR_ECE;
    // TIM17 general 1 ms counter with interrupt
    TIM17->PSC = (expected_core_clock_hz / 1000) - 1;
    TIM17->ARR = cfg::update_rate_ms - 1;
    TIM17->CNT = 0;
    TIM17->EGR |= TIM_EGR_UG;
    TIM17->DIER |= TIM_DIER_UIE;
}

void FrequencyMeter::start()
{
    NVIC_EnableIRQ(TIM17_IRQn);
    TIM1->CNT = 0;
    TIM17->CNT = 0;
    TIM17->CR1 |= TIM_CR1_CEN;
    TIM1->CR1 |= TIM_CR1_CEN;
}

void FrequencyMeter::stop()
{
    TIM17->CR1 &= ~TIM_CR1_CEN;
    TIM1->CR1 &= ~TIM_CR1_CEN;
    NVIC_DisableIRQ(TIM17_IRQn);
}

void FrequencyMeter::irq(void)
{
    if (TIM17->SR & TIM_SR_UIF)
    {
        ready = true;
        result = TIM1->CNT;
        TIM1->CNT = 0;
        TIM17->SR &= ~TIM_SR_UIF;
    }
}

} // namespace fm

extern "C" void TIM17_IRQHandler(void) { fm::FrequencyMeter::instance().irq(); }
