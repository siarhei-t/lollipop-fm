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
    RCC->IOPENR |= RCC_IOPENR_GPIOBEN;
    // PB6, oscillator power control
    GPIOB->MODER &= ~GPIO_MODER_MODE6;
    GPIOB->MODER |= GPIO_MODER_MODE6_0;
    // oscillator enabled by default
    oscSetState(true);
}

void DevicePorts::oscSetState(const bool state)
{
    if (state)
    {
        GPIOB->BSRR = GPIO_BSRR_BR6;
    }
    else
    {
        GPIOB->BSRR = GPIO_BSRR_BS6;
    }
}

} // namespace io
