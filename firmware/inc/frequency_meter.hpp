/**
 * @file frequency_meter.hpp
 *
 * @brief
 *
 */

#ifndef FREQUENCY_METER_H
#define FREQUENCY_METER_H

#include <cstdint>

constexpr std::uint32_t timer_num_of_samples = 2;

/**
 * @brief Class to measure frequency using TIM1 and DMA on STM32C0.
 *
 * This is a singleton class that manages the hardware timer, DMA, and capture buffer.
 * Only one instance of this class should exist in the system.
 */
class FrequencyMeter
{
public:
    /**
     * @brief Get the singleton instance of FrequencyMeter.
     *
     * The object is created on first call (Meyers singleton) and lives for the
     * entire runtime of the program.
     *
     * @return Reference to the single FrequencyMeter instance.
     */
    static FrequencyMeter& instance();

    /**
     * @brief Start frequency measurement.
     *
     * Enables DMA and TIM1 counter to start capturing events.
     */
    void start();

    /**
     * @brief Stop frequency measurement.
     *
     * Disables TIM1 counter and DMA channel to stop capturing events.
     */
    void stop();

    /**
     * @brief Get the last measured frequency value.
     *
     * @return The difference between captured timer samples.
     */
    std::uint32_t getValue() const { return result; }

    /**
     * @brief DMA interrupt handler for TIM1 channel 4.
     *
     * This function should be called from the actual DMA ISR.
     * It updates the measurement result from the DMA buffer.
     */
    void irq();

private:
    /**
     * @brief Construct a new FrequencyMeter object.
     *
     * Initializes hardware registers, GPIO, TIM1, and DMA.
     * Constructor is private to enforce singleton pattern.
     */
    FrequencyMeter();

    /// Deleted copy constructor to prevent copying.
    FrequencyMeter(const FrequencyMeter&) = delete;

    /// Deleted copy assignment operator to prevent copying.
    FrequencyMeter& operator=(const FrequencyMeter&) = delete;

    /// Circular buffer storing captured timer values.
    std::uint32_t buffer[timer_num_of_samples];

    /// Last measured value, updated by IRQ. Volatile because modified in ISR.
    volatile std::uint32_t result = 0;
};

#endif // FREQUENCY_METER_H