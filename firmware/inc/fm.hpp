/**
 * @file fm.hpp
 *
 * @brief
 *
 */

#ifndef FM_H
#define FM_H

#include <cstdint>
namespace fm
{

constexpr std::uint32_t timer_num_of_samples = 4;

class FrequencyMeter
{
public:
    static FrequencyMeter& instance();
    void start();
    void stop();
    std::uint32_t getValue() const { return result; }
    void irq();

private:
    FrequencyMeter();
    FrequencyMeter(const FrequencyMeter&) = delete;
    FrequencyMeter& operator=(const FrequencyMeter&) = delete;
    std::uint16_t buffer[timer_num_of_samples];
    volatile std::uint32_t result = 0;
};

} // namespace fm
#endif // FREQUENCY_METER_H