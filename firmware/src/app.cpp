/**
 * @file app.cpp
 *
 * @brief tbd
 *
 *
 */

#include "app.hpp"
#include "buzzer.hpp"
#include "config.hpp"
#include "frequency_meter.hpp"
#include "io.hpp"
#include "led.hpp"
#include "serial.hpp"
#include <cstdint>

namespace app
{

led::LedControl& led_ctrl = led::LedControl::instance();
serial::Serial& sp = serial::Serial::instance();
fm::FrequencyMeter& fm = fm::FrequencyMeter::instance();
io::DevicePorts& io = io::DevicePorts::instance();
bz::Buzzer& bz = bz::Buzzer::instance();

static inline std::uint32_t local_fabs(const std::uint32_t a, const std::uint32_t b)
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

static inline std::uint32_t calibration()
{
    std::uint32_t cap = 0;
    std::uint32_t reference = 0;

    for (int i = 0; i < cfg::initial_num_of_samples; ++i)
    {
        cap += fm.getValue();
        sp.print("cap = %d\n", cap);
        vTaskDelay(pdMS_TO_TICKS(cfg::update_rate_ms));
    }
    reference = cap / cfg::initial_num_of_samples;
    return reference;
}

Application& Application::instance()
{
    static Application app;
    return app;
}

void Application::start()
{
    // create main task
    task_handle = xTaskCreateStatic(&Application::appTask, "app", stack_size, nullptr, task_priority, stack, &task_buffer);
    // create led task
    led_ctrl.init();
    // enable oscillator
    fm.start();
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

        if (!calibrated || io.checkButtonEvent())
        {
            sp.print("calibration...\n");
            reference = calibration();
            calibrated = true;
            sp.print("calibration done!\n");
            sp.print("reference = %d\n", reference);
        }

        captured_value = fm.getValue();
        deviation = local_fabs(reference, captured_value);
        if ((tick_counter % cfg::log_timeout) == 0)
        {
            sp.print("captured value : %d, deviation : %d \n", captured_value, deviation);
        }
        // logic for deviation
        do
        {
            if (deviation > cfg::allowable_deviation)
            {
                ++deviation_counter;
            }
            else
            {
                deviation_counter = 0;
                led_ctrl.setBlinkMode(led::Blink::off);
                bz.stop();
                //  place for PWM sound off
                break;
            }
            if (deviation_counter > cfg::num_of_deviations)
            {
                // looks like we have something
                if (captured_value < reference)
                {
                    // color metal
                    led_ctrl.setBlinkMode(led::Blink::yellow);
                    // place for PWM color metal sound call
                    bz.setFrequency(3000);
                    bz.start();
                }
                else
                {
                    // black metal
                    led_ctrl.setBlinkMode(led::Blink::yellow);
                    // place for PWM black metal sound call
                    bz.setFrequency(1000);
                    bz.start();
                }
            }
        } while (0);

        vTaskDelay(pdMS_TO_TICKS(cfg::update_rate_ms));
    }
}

} // namespace app

/* ------------------------------ end of file ------------------------------- */
