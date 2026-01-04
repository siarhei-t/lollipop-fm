/**
 * @file app.cpp
 *
 * @brief tbd
 *
 *
 */

#include "app.hpp"
#include "frequency_meter.hpp"
#include "led.hpp"
#include "serial.hpp"

namespace app
{

led::LedControl& led_ctrl = led::LedControl::instance();
serial::Serial& sp = serial::Serial::instance();
fm::FrequencyMeter& fm = fm::FrequencyMeter::instance();

Application& Application::instance()
{
    static Application app;
    return app;
}

void Application::appTask(void* pvParameters)
{
    (void)(pvParameters);
    for (;;)
    {
        sp.print("running...\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void Application::start()
{
    // create main task
    task_handle = xTaskCreateStatic(&Application::appTask, "application", stack_size, nullptr, task_priority, stack, &task_buffer);
    // create led task
    led_ctrl.init();
    led_ctrl.setBlinkMode(led::Blink::yellow);
}

} // namespace app

/* ------------------------------ end of file ------------------------------- */
