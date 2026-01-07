/**
 * @file led.hpp
 *
 * @brief
 *
 */

#ifndef LED_H
#define LED_H

#include "FreeRTOS.h"
#include "task.h"

namespace led
{

/// Stack size for LED control FreeRTOS task (in words, not bytes).
constexpr int stack_size = 64;

/// Priority of LED control task.
/// Chosen slightly below maximum to avoid starving critical system tasks.
constexpr int task_priority = configMAX_PRIORITIES - 3;

/**
 * @brief Enumeration of available LEDs on the board.
 */
enum class Leds
{
    all,   ///< All LEDs
    green, ///< Green LED
    yellow ///< Yellow LED
};

/**
 * @brief LED blinking modes handled by the LED control task.
 */
enum class Blink
{
    yellow, ///< Blink yellow LED
    green,  ///< Blink green LED
    off,    ///< Turn all LEDs off
    both    ///< Blink both LEDs alternately or simultaneously
};

/**
 * @brief Singleton class for controlling board LEDs using a FreeRTOS task.
 *
 * This class encapsulates:
 * - GPIO configuration for LEDs
 * - LED control logic
 * - A statically allocated FreeRTOS task responsible for blinking behavior
 *
 * The task is created using static allocation to avoid dynamic memory usage
 * and ensure deterministic behavior in embedded environments.
 *
 * Only one instance of this class can exist (singleton pattern).
 */
class LedControl
{
public:
    /**
     * @brief Get the singleton instance of LedControl.
     *
     * The instance is created on first use.
     *
     * @return Reference to the single LedControl instance.
     */
    static LedControl& instance();

    /**
     * @brief Create the LED control task.
     *
     * Creates and starts a statically allocated FreeRTOS task
     * responsible for LED blinking according to the selected mode.
     *
     * This function should be called once during system initialization,
     * after the scheduler is ready to run.
     */
    void init();

    /**
     * @brief Set the current LED blinking mode.
     *
     * The mode is read by the LED control task and applied periodically.
     *
     * @param mode Desired blinking mode.
     */
    void setBlinkMode(const Blink mode, const bool yellow = false, const bool green = false);

    /**
     * @brief FreeRTOS task entry function for LED control.
     *
     * This function implements the main LED control loop and should not
     * be called directly. It is passed to xTaskCreateStatic().
     *
     * @param pvParameters Pointer to LedControl instance.
     */
    static void ledTask(void* pvParameters);

private:
    /**
     * @brief Construct a new LedControl object.
     *
     * Initializes GPIO pins required for LED control.
     * The constructor is private to enforce the singleton pattern.
     */
    LedControl();

    /**
     * @brief Turn on the specified LED.
     *
     * @param led LED to enable.
     */
    void enable(const led::Leds led);

    /**
     * @brief Turn off the specified LED.
     *
     * @param led LED to disable.
     */
    void disable(const led::Leds led);

    /**
     * @brief Toggle the specified LED.
     *
     * If the LED is currently on, it will be turned off.
     * If it is off, it will be turned on.
     *
     * @param led LED to toggle.
     */
    void toggle(const led::Leds led);

    /// Deleted copy constructor to prevent copying.
    LedControl(const LedControl&) = delete;

    /// Deleted copy assignment operator to prevent copying.
    LedControl& operator=(const LedControl&) = delete;

    /// Current blinking mode, accessed by the LED task.
    Blink blink_mode = Blink::off;

    /// Stack memory for the LED control task (static allocation).
    StackType_t stack[stack_size];

    /// Static task control block for the LED control task.
    StaticTask_t task_buffer;

    /// Handle of the LED control FreeRTOS task.
    TaskHandle_t task_handle = nullptr;
};

} // namespace led

#endif // LED_H