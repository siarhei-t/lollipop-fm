/**
 * @file led.hpp
 *
 * @brief
 *
 */

#ifndef LED_H
#define LED_H

#include "FreeRTOS.h"
#include "task.h"

namespace led
{

constexpr int stack_size = configMINIMAL_STACK_SIZE;
constexpr int task_priority = configMAX_PRIORITIES - 2;

enum class Leds
{
    All,
    Ferrite,
    NoneFerrite
};

enum class Blink
{
    Ferrite,     ///< Blink LED for ferrite indication
    NoneFerrite, ///< Blink LED for non ferrite indication
    Off,         ///< Turn all LEDs off
    Both         ///< Blink both LEDs alternately or simultaneously
};

class LedControl
{
public:
    static LedControl& instance();
    void init();
    void setBlinkMode(const Blink mode, const bool f = false, const bool non_f = false);
    static void ledTask(void* pvParameters);

private:
    LedControl();

    void enable(const led::Leds led);
    void disable(const led::Leds led);
    void toggle(const led::Leds led);

    LedControl(const LedControl&) = delete;
    LedControl& operator=(const LedControl&) = delete;
    Blink blink_mode = Blink::Off;
    // FreeRTOS static task variables
    StackType_t stack[stack_size];
    StaticTask_t task_buffer;
    TaskHandle_t task_handle = nullptr;
};

} // namespace led

#endif // LED_H