/**
 * @file config.hpp
 *
 * @brief
 *
 */

#ifndef CONFIG_H
#define CONFIG_H

namespace cfg
{
////////////////////////DETECTOR SETTINGS AND CONSTANTS/////////////////////////////
constexpr int log_period_ms = 2000;         // if project build with serial port support
constexpr int update_rate_ms = 50;          // the period at which the frequency meter is polled
constexpr int main_task_delay = 1;          // controller main task delay
constexpr int drift_limit = 200;            // frequency drift max value for drift counter
constexpr int initial_num_of_samples = 50;  // the number of frequency meter measurements to obtain the average value during calibration
constexpr int calibration_trim_samples = 5; // the number of max and min values removed from the calibration array
constexpr int allowable_deviation = 2;      // maximum permitted deviation in absolute values ​​of the timer counter
constexpr int num_of_deviations = 2;        // maximum number of consecutive deviations permitted
////////////////////////////////////////////////////////////////////////////////
} // namespace cfg

#endif // CONFIG_H