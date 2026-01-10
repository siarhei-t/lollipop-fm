/**
 * @file buzzer.hpp
 *
 * @brief
 *
 */

#ifndef BUZZER_H
#define BUZZER_H

#include <cstdint>

namespace bz
{

constexpr std::uint32_t default_frequency_hz = 1000;

class Buzzer
{
public:
    static Buzzer& instance();
    void start();
    void stop();
    void setFrequency(const std::uint16_t frequency_hz);

private:
    Buzzer();
    /// Deleted copy constructor to prevent copying.
    Buzzer(const Buzzer&) = delete;

    /// Deleted copy assignment operator to prevent copying.
    Buzzer& operator=(const Buzzer&) = delete;
    std::uint16_t frequency_hz = default_frequency_hz;
};

} // namespace bz

#endif // BUZZER_H