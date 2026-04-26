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
    RCC->APBENR1 |= RCC_APBENR1_USART2EN;
    // LOLLIPOP-FM rev. 1.0 board
    // PA4 AF1, PA5 AF1
    GPIOA->MODER &= ~(GPIO_MODER_MODE5 | GPIO_MODER_MODE4);
    GPIOA->MODER |= (GPIO_MODER_MODE5_1 | GPIO_MODER_MODE4_1);
    GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED5 | GPIO_OSPEEDR_OSPEED4;
    GPIOA->AFR[0] &= ~(0xF << GPIO_AFRL_AFSEL5_Pos);
    GPIOA->AFR[0] &= ~(0xF << GPIO_AFRL_AFSEL4_Pos);
    GPIOA->AFR[0] |= (0x1 << GPIO_AFRL_AFSEL5_Pos);
    GPIOA->AFR[0] |= (0x1 << GPIO_AFRL_AFSEL4_Pos);
    // USART2, 57600 baudrate
    USART2->CR1 &= ~USART_CR1_UE;
    USART2->BRR = baudrate_57600;
    USART2->CR1 |= USART_CR1_TE | USART_CR1_RE;
    USART2->CR1 |= USART_CR1_UE;
}

int Serial::print(const char* format, ...)
{
    taskENTER_CRITICAL();
    va_list args;
    va_start(args, format);
    int len = std::vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    if (len > 0)
    {
        send(buffer, len);
    }
    taskEXIT_CRITICAL();
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
    while (!(USART2->ISR & USART_ISR_TXE_TXFNF))
    {
        taskYIELD();
    }
    USART2->TDR = c;
}

} // namespace serial

/* ------------------------------ end of file ------------------------------- */
