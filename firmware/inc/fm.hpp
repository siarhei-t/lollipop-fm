/**
 * @file fm.hpp
 *
 * @brief
 *
 */

#ifndef FM_H
#define FM_H

#include "config.hpp"

namespace fm
{
class FrequencyMeter
{
public:
    static FrequencyMeter& instance();
    void start();
    void stop();
    bool isReady() const { return ready; }
    std::uint32_t getValue();
    std::uint16_t getPeriods() const { return value_periods; }
    bool isFault() const { return fault; }
    void setPeriods(std::uint16_t periods);
    void irq();

private:
    FrequencyMeter();
    FrequencyMeter(const FrequencyMeter&) = delete;
    FrequencyMeter& operator=(const FrequencyMeter&) = delete;

    void onCapture(std::uint16_t t);
    void onTimeout();
    void startWindow();
    void publish(std::uint32_t ticks);

    // active window (executed inside interrupt)
    bool have_last = false;
    std::uint16_t last_capture = 0;
    std::uint32_t win_ticks = 0;
    std::uint16_t captures_left = 0;
    std::uint16_t window_periods = cfg::periods_default;
    std::uint64_t window_scaled = 0;
    volatile std::uint16_t new_periods = 0;
    // result generation (executed inside interrupt)
    volatile bool ready = false;
    volatile std::uint32_t res_ticks = 0;
    volatile std::uint16_t res_periods = 0;
    volatile bool res_fault = false;
    std::uint8_t streak = 0;
    // status
    std::uint16_t value_periods = 0;
    bool fault = false;
};

} // namespace fm
#endif // FREQUENCY_METER_H