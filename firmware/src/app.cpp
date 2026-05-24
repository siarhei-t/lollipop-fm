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
#include "fm.hpp"
#include "io.hpp"
#include "led.hpp"
#include "serial.hpp"
#include <cstdint>

namespace app
{

led::LedControl& led = led::LedControl::instance();
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
        do
        {
            vTaskDelay(pdMS_TO_TICKS(1));
        } while (!fm.isReady());
        cap += fm.getValue();
        uint32_t progress = (i * 100) / cfg::initial_num_of_samples;
        sp.print(" progress : %d %%\r", progress);
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
    // create and start led task control
    led.init();
    // create and start buzzer task control
    bz.init();
    // enable oscillator
    io.oscSetState(true);
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

    sp.print("*****************************\n");
    for (int i = 0; i < 2; ++i)
    {
        sp.print(".\n");
    }
    sp.print("Lollipop-FM firmware started.\n");
    sp.print("version : %s \n", fw_version);
    for (int i = 0; i < 2; ++i)
    {
        sp.print(".\n");
    }
    sp.print("*****************************\n");
    for (;;)
    {
        ++tick_counter;

        if (io.checkButtonEvent())
        {
            sp.print("calibration...\n");
            reference = calibration();
            calibrated = true;
            sp.print("calibration done!\n");
            sp.print("reference = %d\n", reference);
        }

        if (!calibrated)
        {
            continue;
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
                led.setBlinkMode(led::Blink::Off);
                bz.stopPlaying();
                break;
            }
            if (deviation_counter > cfg::num_of_deviations)
            {
                // looks like we have something
                if (captured_value < reference)
                {
                    // non ferrite metal
                    led.setBlinkMode(led::Blink::NoneFerrite);
                    bz.playFerrite();
                }
                else
                {
                    // ferrite metal
                    led.setBlinkMode(led::Blink::Ferrite);
                    bz.playNoneFerrite();
                }
            }
        } while (0);
        vTaskDelay(pdMS_TO_TICKS(cfg::update_rate_ms));
    }
}

} // namespace app

/* ------------------------------ end of file ------------------------------- */
