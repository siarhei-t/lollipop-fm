/**
 * @file frequency_meter.cpp
 *
 * @brief
 *
 */

#include "frequency_meter.hpp"
#include <stm32c011xx.h>

namespace fm
{

// ID from rm0490 form DMAMUX config
constexpr std::uint32_t tim1_channel4_id = 23;
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
    RCC->IOPENR |= RCC_IOPENR_GPIOBEN;
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    // PB6, oscillator power control
    GPIOB->MODER &= ~GPIO_MODER_MODE6;
    GPIOB->MODER |= GPIO_MODER_MODE6_0;
    // PA3 AF5 TIM1_CH4
    GPIOA->MODER &= ~GPIO_MODER_MODE3;
    GPIOA->MODER |= GPIO_MODER_MODE3_1;
    GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL3;
    GPIOA->AFR[0] |= (5U << GPIO_AFRL_AFSEL3_Pos);
    // TIM1 CH4 input capture
    TIM1->PSC = 0;
    TIM1->ARR = counter_top_value;
    TIM1->CCMR2 |= TIM_CCMR2_CC4S_0;
    TIM1->CCER |= TIM_CCER_CC4E;
    TIM1->DIER |= TIM_DIER_CC4DE;
    // DMA , channel 1, connected to TIM1 Channel 4
    DMA1_Channel1->CCR &= ~DMA_CCR_EN;
    DMAMUX1_Channel0->CCR = (tim1_channel4_id << DMAMUX_CxCR_DMAREQ_ID_Pos);
    DMA1_Channel1->CPAR = (uint32_t)&TIM1->CCR4;
    DMA1_Channel1->CMAR = (uint32_t)buffer;
    DMA1_Channel1->CNDTR = timer_num_of_samples;
    DMA1_Channel1->CCR = DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0 | DMA_CCR_CIRC | DMA_CCR_TCIE | DMA_CCR_MINC;
    NVIC_EnableIRQ(DMA1_Channel1_IRQn);
}

void FrequencyMeter::start()
{
    GPIOB->BSRR = GPIO_BSRR_BR6;
    DMA1_Channel1->CCR |= DMA_CCR_EN;
    TIM1->CR1 |= TIM_CR1_CEN;
}

void FrequencyMeter::stop()
{
    GPIOB->BSRR = GPIO_BSRR_BS6;
    TIM1->CR1 &= ~TIM_CR1_CEN;
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
