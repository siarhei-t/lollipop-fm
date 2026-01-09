/**
 * @file app.cpp
 *
 * @brief tbd
 *
 *
 */

#include "app.hpp"
#include "frequency_meter.hpp"
#include "io.hpp"
#include "led.hpp"
#include "serial.hpp"
#include <cstdint>

namespace app
{

////////////////////////DETECTOR SETTINGS AND CONSTANTS/////////////////////////////
constexpr int log_period_ms = 2000;                         // if project build with serial port support
constexpr int update_rate_ms = 50;                          // the period at which the frequency meter is polled
constexpr int num_of_samples_calibration = 20;              // the number of frequency meter measurements to obtain the average value during calibration
constexpr int allowable_deviation = 1;                      // maximum permitted deviation in absolute values ​​of the timer counter
constexpr int num_of_deviations = 2;                        // maximum number of consecutive deviations permitted
constexpr int log_timeout = log_period_ms / update_rate_ms; // period for serial port output
////////////////////////////////////////////////////////////////////////////////

led::LedControl& led_ctrl = led::LedControl::instance();
serial::Serial& sp = serial::Serial::instance();
fm::FrequencyMeter& fm = fm::FrequencyMeter::instance();
io::DevicePorts& io = io::DevicePorts::instance();

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

static inline uint32_t calibration()
{
    uint32_t cap = 0;
    uint32_t reference = 0;
    vTaskDelay(pdMS_TO_TICKS(1000));
    for (int i = 0; i < num_of_samples_calibration; ++i)
    {
        cap += fm.getValue();
        sp.print("cap = %d\n", cap);
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

void Application::start()
{
    // create main task
    task_handle = xTaskCreateStatic(&Application::appTask, "application", stack_size, nullptr, task_priority, stack, &task_buffer);
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
        if ((tick_counter % log_timeout) == 0)
        {
            sp.print("captured value : %d, deviation : %d \n", captured_value, deviation);
        }
        // logic for deviation
        do
        {
            if (deviation > allowable_deviation)
            {
                ++deviation_counter;
            }
            else
            {
                deviation_counter = 0;
                led_ctrl.setBlinkMode(led::Blink::off);
                // place for PWM sound off
                break;
            }
            if (deviation_counter > num_of_deviations)
            {
                // looks like we have something
                if (captured_value < reference)
                {
                    // color metal
                    led_ctrl.setBlinkMode(led::Blink::yellow);
                    // place for PWM color metal sound call
                }
                else
                {
                    // black metal
                    led_ctrl.setBlinkMode(led::Blink::yellow);
                    // place for PWM black metal sound call
                }
            }
        } while (0);

        vTaskDelay(pdMS_TO_TICKS(update_rate_ms));
    }
}

} // namespace app

/* ------------------------------ end of file ------------------------------- */
