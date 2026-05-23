/**
 * @file io.cpp
 *
 * @brief
 *
 */

#include "io.hpp"
#include <stm32c011xx.h>

namespace io
{

DevicePorts& DevicePorts::instance()
{
    static DevicePorts fm;
    return fm;
}

DevicePorts::DevicePorts()
{
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;

    // LOLLIPOP-FM rev. 1.0 board
    // PA11, oscillator power control
    GPIOA->MODER &= ~GPIO_MODER_MODE11;
    GPIOA->MODER |= GPIO_MODER_MODE11_0;
    // PA1 input and interrupt for control button
    GPIOA->MODER &= ~GPIO_MODER_MODE1;
    // falling edge
    EXTI->RTSR1 &= ~EXTI_RTSR1_RT1;
    EXTI->FTSR1 |= EXTI_FTSR1_FT1;
    // PA by default
    EXTI->EXTICR[0] &= ~EXTI_EXTICR1_EXTI1;
    EXTI->IMR1 |= EXTI_IMR1_IM1;
    EXTI->FPR1 = EXTI_FPR1_FPIF1;
    NVIC_EnableIRQ(EXTI0_1_IRQn);
}

bool DevicePorts::checkButtonEvent()
{
    if (button_pressed)
    {
        button_pressed = false;
        return true;
    }
    else
    {
        return false;
    }
}

void DevicePorts::oscSetState(const bool state)
{
    if (state)
    {
        GPIOA->BSRR = GPIO_BSRR_BR11;
    }
    else
    {
        GPIOA->BSRR = GPIO_BSRR_BS11;
    }
}

void DevicePorts::irq()
{
    if (EXTI->FPR1 & EXTI_FPR1_FPIF1)
    {
        EXTI->FPR1 = EXTI_FPR1_FPIF1;
        button_pressed = true;
    }
}

} // namespace io

extern "C" void EXTI0_1_IRQHandler(void) { io::DevicePorts::instance().irq(); }
