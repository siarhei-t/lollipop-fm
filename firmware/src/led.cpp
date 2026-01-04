/**
 * @file led.cpp
 *
 * @brief tbd
 *
 *
 */

#include "led.hpp"
#include <stm32c011xx.h>

namespace led
{

LedControl& LedControl::instance()
{
    static LedControl l;
    return l;
}

LedControl::LedControl()
{
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
    // GREEN led at PA5 and YELLOW led at PA6
    GPIOA->MODER &= ~GPIO_MODER_MODE5;
    GPIOA->MODER &= ~GPIO_MODER_MODE6;
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
        case Leds::all_leds:
        default:
            GPIOA->BSRR |= GPIO_BSRR_BR5;
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
        case Leds::all_leds:
        default:
            GPIOA->BSRR |= GPIO_BSRR_BS5;
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
            {
                GPIOA->BSRR = GPIO_BSRR_BR5;
            }
            else
            {
                GPIOA->BSRR = GPIO_BSRR_BS5;
            }
            break;

        case Leds::led_yellow:
            if (GPIOA->ODR & GPIO_ODR_OD6)
            {
                GPIOA->BSRR = GPIO_BSRR_BR6;
            }
            else
            {
                GPIOA->BSRR = GPIO_BSRR_BS6;
            }
            break;
        case Leds::all_leds:
        default:
            if (GPIOA->ODR & GPIO_ODR_OD5)
            {
                GPIOA->BSRR = GPIO_BSRR_BR5;
            }
            else
            {
                GPIOA->BSRR = GPIO_BSRR_BS5;
            }
            if (GPIOA->ODR & GPIO_ODR_OD6)
            {
                GPIOA->BSRR = GPIO_BSRR_BR6;
            }
            else
            {
                GPIOA->BSRR = GPIO_BSRR_BS6;
            }
            break;
    }
}

void LedControl::ledTask(void* pvParameters)
{
    (void)(pvParameters);

    for (;;)
    {
        switch (instance().blink_mode)
        {
            case Blink::green:
                instance().disable(Leds::led_yellow);
                instance().toggle(Leds::led_green);
                vTaskDelay(pdMS_TO_TICKS(100));
                break;
            case Blink::yellow:
                instance().disable(Leds::led_green);
                instance().toggle(Leds::led_yellow);
                vTaskDelay(pdMS_TO_TICKS(100));
                break;
            case Blink::both:
                instance().toggle(Leds::led_yellow);
                instance().toggle(Leds::led_green);
                vTaskDelay(pdMS_TO_TICKS(100));
                break;
            case Blink::off:
            default:
                instance().disable(Leds::led_green);
                instance().disable(Leds::led_yellow);
                vTaskDelay(pdMS_TO_TICKS(50));
                break;
        }
    }
}

void LedControl::start()
{
    task_handle = xTaskCreateStatic(&LedControl::ledTask, "led control", led::stack_size, nullptr, led::task_priority, stack, &task_buffer);
}

} // namespace led

/* ------------------------------ end of file ------------------------------- */
