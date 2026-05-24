/**
 * @file led.cpp
 *
 * @brief tbd
 *
 *
 */

#include "led.hpp"
#include <cstdint>
#include <stm32c011xx.h>

namespace led
{

constexpr std::uint32_t ferrite_out_reg_mask = GPIO_ODR_OD8;
constexpr std::uint32_t non_ferrite_out_reg_mask = GPIO_ODR_OD7;
constexpr std::uint32_t ferrite_bit_set_mask = GPIO_BSRR_BS8;
constexpr std::uint32_t ferrite_bit_reset_mask = GPIO_BSRR_BR8;
constexpr std::uint32_t non_ferrite_bit_set_mask = GPIO_BSRR_BS7;
constexpr std::uint32_t non_ferrite_bit_reset_mask = GPIO_BSRR_BR7;

constexpr int blink_task_delay = 100;

LedControl& LedControl::instance()
{
    static LedControl l;
    return l;
}

LedControl::LedControl()
{
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
    // LOLLIPOP-FM rev. 1.0 board
    // ferrite indication led at PA8 and non ferrite led indication at PA7
    // outputs, no pullup/pulldown, low speed
    GPIOA->MODER &= ~GPIO_MODER_MODE7;
    GPIOA->MODER &= ~GPIO_MODER_MODE8;
    GPIOA->MODER |= (GPIO_MODER_MODE7_0 | GPIO_MODER_MODE8_0);
    disable(Leds::All);
}

void LedControl::setBlinkMode(const Blink mode, const bool f, const bool non_f)
{
    if (f)
    {
        enable(Leds::Ferrite);
    }
    else
    {
        disable(Leds::Ferrite);
    }
    if (non_f)
    {
        enable(Leds::NoneFerrite);
    }
    else
    {
        disable(Leds::NoneFerrite);
    }
    blink_mode = mode;
}

void LedControl::enable(const Leds led)
{
    switch (led)
    {
        case Leds::Ferrite:
            GPIOA->BSRR |= ferrite_bit_reset_mask;
            break;
        case Leds::NoneFerrite:
            GPIOA->BSRR |= non_ferrite_bit_reset_mask;
            break;
        case Leds::All:
        default:
            GPIOA->BSRR |= ferrite_bit_reset_mask;
            GPIOA->BSRR |= non_ferrite_bit_reset_mask;
            break;
    }
}

void LedControl::disable(const Leds led)
{
    switch (led)
    {
        case Leds::Ferrite:
            GPIOA->BSRR |= ferrite_bit_set_mask;
            break;
        case Leds::NoneFerrite:
            GPIOA->BSRR |= non_ferrite_bit_set_mask;
            break;
        case Leds::All:
        default:
            GPIOA->BSRR |= ferrite_bit_set_mask;
            GPIOA->BSRR |= non_ferrite_bit_set_mask;
            break;
    }
}

void LedControl::toggle(const Leds led)
{
    switch (led)
    {
        case Leds::Ferrite:
            if (GPIOA->ODR & ferrite_out_reg_mask)
            {
                GPIOA->BSRR = ferrite_bit_reset_mask;
            }
            else
            {
                GPIOA->BSRR = ferrite_bit_set_mask;
            }
            break;

        case Leds::NoneFerrite:
            if (GPIOA->ODR & non_ferrite_out_reg_mask)
            {
                GPIOA->BSRR = non_ferrite_bit_reset_mask;
            }
            else
            {
                GPIOA->BSRR = non_ferrite_bit_set_mask;
            }
            break;
        case Leds::All:
        default:
            if (GPIOA->ODR & ferrite_out_reg_mask)
            {
                GPIOA->BSRR = ferrite_bit_reset_mask;
            }
            else
            {
                GPIOA->BSRR = ferrite_bit_set_mask;
            }
            if (GPIOA->ODR & non_ferrite_out_reg_mask)
            {
                GPIOA->BSRR = non_ferrite_bit_reset_mask;
            }
            else
            {
                GPIOA->BSRR = non_ferrite_bit_set_mask;
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
            case Blink::Ferrite:
                instance().disable(Leds::NoneFerrite);
                instance().toggle(Leds::Ferrite);
                vTaskDelay(pdMS_TO_TICKS(blink_task_delay));
                break;
            case Blink::NoneFerrite:
                instance().disable(Leds::Ferrite);
                instance().toggle(Leds::NoneFerrite);
                vTaskDelay(pdMS_TO_TICKS(blink_task_delay));
                break;
            case Blink::Both:
                instance().toggle(Leds::Ferrite);
                instance().toggle(Leds::NoneFerrite);
                vTaskDelay(pdMS_TO_TICKS(blink_task_delay));
                break;
            case Blink::Off:
            default:
                instance().disable(Leds::Ferrite);
                instance().disable(Leds::NoneFerrite);
                vTaskDelay(pdMS_TO_TICKS(blink_task_delay));
                break;
        }
    }
}

void LedControl::init() { task_handle = xTaskCreateStatic(&LedControl::ledTask, "led control", stack_size, nullptr, task_priority, stack, &task_buffer); }

} // namespace led

/* ------------------------------ end of file ------------------------------- */
