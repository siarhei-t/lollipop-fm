/**
 * @file config.hpp
 *
 * @brief device constants and configurations
 *
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <cstdint>

namespace cfg
{
////////////////////////DETECTOR SETTINGS AND CONSTANTS/////////////////////////////

constexpr std::uint32_t timer_clock_hz = 8000000; // expected timer clock
constexpr std::uint32_t min_hz = 16000;           // min allowed Colpitts generator frequency
constexpr std::uint32_t max_hz = 24000;           // max allowed Colpitts generator frequency
// responsiveness and accuracy settings
constexpr std::uint16_t periods_default = 200;
constexpr std::uint16_t periods_min = 8;
constexpr std::uint16_t periods_max = 40000; // ≈ 2s in case of 20KHz
constexpr std::uint8_t fault_debounce = 3;
// input capture divider ICPSC = /8
constexpr std::uint32_t capture_shift = 3;
constexpr std::uint32_t edges_per_capture = 1U << capture_shift;
// WDT, 0xFFFF / 2
constexpr std::uint16_t timeout_ticks = 0x8000;

constexpr int main_task_delay = 1;          // controller main task delay
constexpr int drift_limit = 2000;           // frequency drift max value for drift counter
constexpr int initial_num_of_samples = 50;  // the number of frequency meter measurements to obtain the average value during calibration
constexpr int calibration_trim_samples = 5; // the number of max and min values removed from the calibration array
constexpr int allowable_deviation = 2;      // maximum permitted deviation in absolute values of the timer counter
constexpr int num_of_deviations = 2;        // maximum number of consecutive deviations permitted

static_assert(edges_per_capture == 8, "IC1PSC in constructor is configured for /8");
static_assert((periods_default & (edges_per_capture - 1)) == 0, "must be multiple of 8");
static_assert(std::uint64_t(edges_per_capture) * timer_clock_hz < std::uint64_t(min_hz) * timeout_ticks, "timeout too short for this clock");

////////////////////////////////////////////////////////////////////////////////
} // namespace cfg

#endif // CONFIG_H