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
#include <cstdint>

namespace app
{

constexpr int log_period_ms = 2000;
constexpr int update_rate_ms = 50;
constexpr int num_of_samples_calibration = 20;
constexpr int num_of_deviations = 2;
constexpr int log_timeout = log_period_ms / update_rate_ms;

led::LedControl& led_ctrl = led::LedControl::instance();
serial::Serial& sp = serial::Serial::instance();
fm::FrequencyMeter& fm = fm::FrequencyMeter::instance();

static inline uint32_t local_fabs(const uint32_t a, const uint32_t b)
{
    if (a >= b)
    {
        return a - b;
    }
    else
    {
        return b - a;
    }
}

static uint32_t calibration()
{
    uint32_t cap = 0;
    uint32_t reference = 0;
    for (int i = 0; i < num_of_samples_calibration; ++i)
    {
        cap += fm.getValue();
        vTaskDelay(pdMS_TO_TICKS(update_rate_ms));
    }
    reference = cap / num_of_samples_calibration;
    return reference;
}

Application& Application::instance()
{
    static Application app;
    return app;
}

void Application::appTask(void* pvParameters)
{
    (void)(pvParameters);
    static bool calibrated = false;
    static uint32_t tick_counter = 0;
    static uint32_t reference = 0;
    static uint32_t deviation = 0;
    static uint32_t captured_value = 0;
    static uint32_t deviation_counter = 0;

    for (;;)
    {
        ++tick_counter;

        if (!calibrated)
        {
            sp.print("calibration...\n");
            reference = calibration();
            calibrated = true;
            sp.print("calibration done!\n");
            sp.print("reference = %d\n", reference);
        }

        captured_value = fm.getValue();
        deviation = local_fabs(reference, captured_value);
        if ((tick_counter % log_timeout) == 0)
        {
            sp.print("captured value : %d, deviation : %d \n", captured_value, deviation);
        }
        // logic for deviation

        //
        vTaskDelay(pdMS_TO_TICKS(update_rate_ms));
    }
}

void Application::start()
{
    // create main task
    task_handle = xTaskCreateStatic(&Application::appTask, "application", stack_size, nullptr, task_priority, stack, &task_buffer);
    // create led task
    led_ctrl.init();
    led_ctrl.setBlinkMode(led::Blink::yellow);
    // enable oscillator
    fm.start();
}

} // namespace app

/* ------------------------------ end of file ------------------------------- */
