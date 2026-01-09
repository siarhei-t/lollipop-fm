/**
 * @file io.hpp
 *
 * @brief
 *
 */

#ifndef IO_H
#define IO_H

namespace io
{

/**
 * @brief Hardware port control and EXTI handling class.
 *
 * This class provides a singleton interface for low-level access to
 * device GPIO ports and external interrupt (EXTI) events.
 *
 * It is responsible for:
 *  - controlling output signals (e.g. oscillator enable/disable)
 *  - handling button press events triggered by EXTI interrupts
 *  - providing a safe polling interface for the application layer
 *
 * The class is designed for bare-metal usage without HAL and assumes
 * that the EXTI interrupt handler forwards control to @ref irq().
 */
class DevicePorts
{

public:
    /**
     * @brief Get the singleton instance of DevicePorts.
     *
     * This method returns the single global instance of the class.
     * The instance is created on first use.
     *
     * @return Reference to the DevicePorts instance.
     */
    static DevicePorts& instance();

    /**
     * @brief Set oscillator output state.
     *
     * Enables or disables the oscillator-related GPIO output.
     *
     * @param state
     *        true  - enable oscillator output
     *        false - disable oscillator output
     */
    void oscSetState(const bool state);

    /**
     * @brief Check whether a button press event occurred.
     *
     * This method returns the current button event state and
     * clears the internal event flag.
     *
     * Intended to be called from the main loop (non-interrupt context).
     *
     * @return true if a button press event was detected since the last call,
     *         false otherwise.
     */
    bool checkButtonEvent();

    /**
     * @brief EXTI interrupt handler callback.
     *
     * This method must be called from the actual EXTI IRQ handler.
     * It processes the interrupt source and updates internal state.
     *
     * Example usage:
     * @code
     * void EXTI0_1_IRQHandler(void)
     * {
     *     DevicePorts::instance().irq();
     * }
     * @endcode
     */
    void irq();

private:
    /**
     * @brief Private constructor.
     *
     * Prevents direct instantiation. Use @ref instance() instead.
     */
    DevicePorts();

    /**
     * @brief Deleted copy constructor.
     *
     * Copying is not allowed for this singleton class.
     */
    DevicePorts(const DevicePorts&) = delete;

    /**
     * @brief Deleted copy assignment operator.
     *
     * Assignment is not allowed for this singleton class.
     */
    DevicePorts& operator=(const DevicePorts&) = delete;

    /**
     * @brief Button press event flag.
     *
     * Set inside the EXTI interrupt context and cleared
     * when read by @ref checkButtonEvent().
     */
    volatile bool button_pressed;
};

} // namespace io

#endif // IO_H