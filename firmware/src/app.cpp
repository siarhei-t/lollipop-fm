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
#include <utility>

namespace app
{

led::LedControl& led = led::LedControl::instance();
serial::Serial& sp = serial::Serial::instance();
fm::FrequencyMeter& fm = fm::FrequencyMeter::instance();
io::DevicePorts& io = io::DevicePorts::instance();
bz::Buzzer& bz = bz::Buzzer::instance();

StateEMA state_ema;

static inline void print_hello()
{
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
}
static inline void on_detect(const int32_t deviation)
{
    sp.print("deviation : %d , reference: %d \n", deviation, state_ema.reference);
    if (deviation > 0)
    {
        led.setBlinkMode(led::Blink::NoneFerrite);
        bz.playNoneFerrite();
    }
    else
    {
        led.setBlinkMode(led::Blink::Ferrite);
        bz.playFerrite();
    }
}

static inline void on_clear()
{
    led.setBlinkMode(led::Blink::Off);
    bz.stopPlaying();
}

static inline void reset_ema(const int32_t value)
{
    state_ema.reference = value;
    state_ema.detect_counter = 0;
}

static inline std::int32_t calibration()
{
    static std::uint32_t samples[cfg::initial_num_of_samples];

    for (int i = 0; i < cfg::initial_num_of_samples; ++i)
    {
        do
        {
            vTaskDelay(pdMS_TO_TICKS(1));
        } while (!fm.isReady());
        samples[i] = fm.getValue();
        uint32_t progress = (i * 100) / cfg::initial_num_of_samples;
        sp.print(" progress : %d %%\r", progress);
    }
    for (int i = 0; i < cfg::initial_num_of_samples - 1; i++)
    {
        for (int j = 0; j < cfg::initial_num_of_samples - i - 1; j++)
        {
            if (samples[j] > samples[j + 1])
            {
                std::swap(samples[j], samples[j + 1]);
            }
        }
    }
    uint32_t cap = 0;
    // trim 10%
    for (int i = cfg::calibration_trim_samples; i < cfg::initial_num_of_samples - cfg::calibration_trim_samples; ++i)
    {
        cap += samples[i];
    }
    return cap / (cfg::initial_num_of_samples - (cfg::calibration_trim_samples * 2));
}

static inline void sample(const int32_t captured_value)
{
    int32_t deviation = captured_value - state_ema.reference;
    int32_t abs_dev = deviation >= 0 ? deviation : -deviation;

    if (abs_dev > cfg::allowable_deviation)
    {
        state_ema.drift_counter = 0;
        if (state_ema.detect_counter < 255)
            ++state_ema.detect_counter;
    }
    else
    {
        ++state_ema.drift_counter;
        state_ema.detect_counter = 0;
    }

    if (state_ema.detect_counter >= cfg::num_of_deviations)
    {
        on_detect(deviation);
    }
    else
    {
        on_clear();
    }

    if (state_ema.drift_counter > cfg::drift_limit)
    {
        bool updated = false;
        state_ema.drift_counter = 0;
        if (captured_value > state_ema.reference)
        {
            ++state_ema.reference;
            updated = true;
        }
        else if (captured_value < state_ema.reference)
        {
            --state_ema.reference;
            updated = true;
        }
        if (updated)
        {
            sp.print("reference updated! New value : %d \n", state_ema.reference);
        }
    }
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
    print_hello();
    for (;;)
    {
        if (io.checkButtonEvent())
        {
            sp.print("calibration...\n");
            std::uint32_t reference = calibration();
            reset_ema(reference);
            calibrated = true;
            sp.print("calibration done, reference : %d , allowable deviation : %d \n", reference, cfg::allowable_deviation);
        }
        if (!calibrated)
        {
            continue;
        }
        do
        {
            vTaskDelay(pdMS_TO_TICKS(1));
        } while (!fm.isReady());
        std::uint32_t captured_value = fm.getValue();
        sample(captured_value);
        vTaskDelay(pdMS_TO_TICKS(cfg::main_task_delay));
    }
}

} // namespace app

/* ------------------------------ end of file ------------------------------- */
