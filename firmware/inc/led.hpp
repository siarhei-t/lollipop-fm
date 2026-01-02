/**
 * @file led.hpp
 *
 * @brief
 *
 */

#ifndef LED_H
#define LED_H

/**
 * @brief Enum for available LEDs on the board.
 */
enum class Leds
{
    led_green, ///< Green LED
    led_yellow ///< Yellow LED
};

/**
 * @brief Singleton class to control board LEDs.
 *
 * Provides methods to enable, disable, and toggle LEDs.
 * This class ensures only one instance exists in the system.
 */
class LedControl
{
public:
    /**
     * @brief Get the singleton instance of LedControl.
     *
     * @return Reference to the single LedControl instance.
     */
    static LedControl& instance();

    /**
     * @brief Turn on the specified LED.
     *
     * @param led LED to enable (green or yellow).
     */
    void enable(const Leds led);

    /**
     * @brief Turn off the specified LED.
     *
     * @param led LED to disable (green or yellow).
     */
    void disable(const Leds led);

    /**
     * @brief Toggle the specified LED.
     *
     * If the LED is currently on, it will be turned off.
     * If it is off, it will be turned on.
     *
     * @param led LED to toggle (green or yellow).
     */
    void toggle(const Leds led);

    void start();

    static void ledTask(void* pvParameters);

private:
    /**
     * @brief Construct a new LedControl object.
     *
     * Initializes GPIOA pins for LED control.
     * Constructor is private to enforce singleton pattern.
     */
    LedControl();

    /// Deleted copy constructor to prevent copying.
    LedControl(const LedControl&) = delete;

    /// Deleted copy assignment operator to prevent copying.
    LedControl& operator=(const LedControl&) = delete;
};

#endif // LED_H