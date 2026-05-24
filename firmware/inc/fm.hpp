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
    volatile std::uint32_t result = 0;
};

} // namespace fm
#endif // FREQUENCY_METER_H