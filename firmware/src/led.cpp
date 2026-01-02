/**
 * @file led.cpp
 *
 * @brief tbd
 *
 *
 */

#include "led.hpp"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include <stm32c011xx.h>

LedControl& LedControl::instance()
{
    static LedControl l;
    return l;
}

LedControl::LedControl()
{
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
    // GREEN led at PA5 and YELLOW led at PA6
    GPIOA->MODER &= ~GPIO_MODER_MODE5_Msk;
    GPIOA->MODER &= ~GPIO_MODER_MODE6_Msk;
    // outputs, no pullup/pulldown, low speed
    GPIOA->MODER |= (GPIO_MODER_MODE5_0 | GPIO_MODER_MODE6_0);
    disable(Leds::led_green);
    disable(Leds::led_yellow);
}

void LedControl::enable(const Leds led)
{
    switch (led)
    {
        case Leds::led_green:
            GPIOA->BSRR |= GPIO_BSRR_BR5;
            break;
        case Leds::led_yellow:
            GPIOA->BSRR |= GPIO_BSRR_BR6;
            break;
    }
}

void LedControl::disable(const Leds led)
{
    switch (led)
    {
        case Leds::led_green:
            GPIOA->BSRR |= GPIO_BSRR_BS5;
            break;
        case Leds::led_yellow:
            GPIOA->BSRR |= GPIO_BSRR_BS6;
            break;
    }
}

void LedControl::toggle(const Leds led)
{
    switch (led)
    {
        case Leds::led_green:
            if (GPIOA->ODR & GPIO_ODR_OD5)
                GPIOA->BSRR = GPIO_BSRR_BR5;
            else
                GPIOA->BSRR = GPIO_BSRR_BS5;
            break;

        case Leds::led_yellow:
            if (GPIOA->ODR & GPIO_ODR_OD6)
                GPIOA->BSRR = GPIO_BSRR_BR6;
            else
                GPIOA->BSRR = GPIO_BSRR_BS6;
            break;
    }
}

void LedControl::ledTask(void* pvParameters)
{
    (void)(pvParameters);

    for (;;)
    {
        instance().toggle(Leds::led_yellow);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void LedControl::start() { xTaskCreate(&LedControl::ledTask, "LED_BLINK", configMINIMAL_STACK_SIZE, nullptr, tskIDLE_PRIORITY + 1, nullptr); }

/* ------------------------------ end of file ------------------------------- */
