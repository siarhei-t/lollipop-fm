/**
 * @file fm.cpp
 *
 * @brief
 *
 */

#include "fm.hpp"
#include <stm32c011xx.h>

namespace fm
{

// ID from rm0490 form DMAMUX config
constexpr std::uint32_t tim1_channel1_id = 20;
// top value for TIM ARR register
constexpr std::uint32_t counter_top_value = 0xFFFF;

FrequencyMeter& FrequencyMeter::instance()
{
    static FrequencyMeter fm;
    return fm;
}

FrequencyMeter::FrequencyMeter()
{
    RCC->APBENR2 |= RCC_APBENR2_TIM1EN;
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    // LOLLIPOP-FM rev. 1.0 board
    // PA0 AF5 TIM1_CH1
    GPIOA->MODER &= ~GPIO_MODER_MODE0;
    GPIOA->MODER |= GPIO_MODER_MODE0_1;
    GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL0;
    GPIOA->AFR[0] |= (5U << GPIO_AFRL_AFSEL0_Pos);
    // TIM1 Channel 1 input capture
    TIM1->PSC = 0;
    TIM1->ARR = counter_top_value;
    TIM1->CCMR1 |= TIM_CCMR1_CC1S_0;
    TIM1->CCER |= TIM_CCER_CC1E;
    TIM1->DIER |= TIM_DIER_CC1DE;
    // DMA , channel 1, connected to TIM1 Channel 1
    DMA1_Channel1->CCR &= ~DMA_CCR_EN;
    DMAMUX1_Channel0->CCR = (tim1_channel1_id << DMAMUX_CxCR_DMAREQ_ID_Pos);
    DMA1_Channel1->CPAR = (uint32_t)&TIM1->CCR1;
    DMA1_Channel1->CMAR = (uint32_t)buffer;
    DMA1_Channel1->CNDTR = timer_num_of_samples;
    DMA1_Channel1->CCR = DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0 | DMA_CCR_CIRC | DMA_CCR_TCIE | DMA_CCR_MINC;
    NVIC_EnableIRQ(DMA1_Channel1_IRQn);
}

void FrequencyMeter::start()
{
    DMA1_Channel1->CCR |= DMA_CCR_EN;
    TIM1->CNT = 0;
    TIM1->CR1 |= TIM_CR1_CEN;
}

void FrequencyMeter::stop()
{
    TIM1->CR1 &= ~TIM_CR1_CEN;
    TIM1->CNT = 0;
    DMA1_Channel1->CCR &= ~DMA_CCR_EN;
}

void FrequencyMeter::irq(void)
{
    if (DMA1->ISR & DMA_ISR_TCIF1)
    {
        DMA1->IFCR = DMA_IFCR_CTCIF1;
        if (buffer[3] >= buffer[2])
        {
            result = buffer[3] - buffer[2];
        }
        else
        {
            result = (counter_top_value + 1) - buffer[2] + buffer[3];
        }
    }
}

} // namespace fm

extern "C" void DMA1_Channel1_IRQHandler(void) { fm::FrequencyMeter::instance().irq(); }
