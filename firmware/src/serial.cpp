/**
 * @file serial.cpp
 *
 * @brief tbd
 *
 *
 */

/* Includes ----------------------------------------------------------------- */

#include "serial.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <stm32c011xx.h>

namespace serial
{

// we expect clock 8MHz, oversampling 16
constexpr std::uint32_t baudrate_57600 = 139;

Serial& Serial::instance()
{
    static Serial s;
    return s;
}

Serial::Serial()
{
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
    RCC->APBENR2 |= RCC_APBENR2_USART1EN;
    RCC->APBENR2 |= RCC_APBENR2_SYSCFGEN;

    // Remap USART1 to PA11 / PA12, 9 and 10 is not available for this chip
    SYSCFG->CFGR1 |= SYSCFG_CFGR1_PA11_RMP | SYSCFG_CFGR1_PA12_RMP;

    GPIOA->MODER &= ~(GPIO_MODER_MODE10 | GPIO_MODER_MODE9);
    GPIOA->MODER |= (GPIO_MODER_MODE10_1 | GPIO_MODER_MODE9_1);
    GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED10 | GPIO_OSPEEDR_OSPEED9;
    GPIOA->AFR[1] &= ~(0xF << GPIO_AFRH_AFSEL10_Pos);
    GPIOA->AFR[1] &= ~(0xF << GPIO_AFRH_AFSEL9_Pos);
    GPIOA->AFR[1] |= (0x1 << GPIO_AFRH_AFSEL10_Pos);
    GPIOA->AFR[1] |= (0x1 << GPIO_AFRH_AFSEL9_Pos);

    USART1->CR1 &= ~USART_CR1_UE;
    USART1->BRR = baudrate_57600;
    USART1->CR1 |= USART_CR1_TE | USART_CR1_RE;
    USART1->CR1 |= USART_CR1_UE;
}

int Serial::print(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    int len = std::vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    if (len > 0)
    {
        send(buffer, len);
    }
    return len;
}

void Serial::send(const char* p_buf, const int size)
{
    for (int i = 0; i < size; ++i)
    {
        putchar(p_buf[i]);
    }
    // carriage return
    putchar('\r');
}

void Serial::putchar(const char c)
{
    while (!(USART1->ISR & USART_ISR_TXE_TXFNF))
    {
        taskYIELD();
    }
    USART1->TDR = c;
}

} // namespace serial

/* ------------------------------ end of file ------------------------------- */
