/**
 * @file serial.hpp
 *
 * @brief
 *
 */

#ifndef SERIAL_H
#define SERIAL_H

namespace serial
{

/// Size of the internal buffer for formatted output
constexpr int buffer_size = 64;

/**
 * @brief UART1 interface class for STM32C011.
 *
 * Provides singleton access to the UART and functions
 * for printing text via `print()` in printf-style.
 *
 * Uses a fixed-size internal buffer for formatting strings.
 */
class Serial
{
public:
    /**
     * @brief Get the singleton instance of the Serial class.
     *
     * @return Reference to the Serial object.
     *
     * Implements the singleton pattern to ensure that
     * only one instance exists for UART communication.
     */
    static Serial& instance();

    /**
     * @brief Print a formatted string to the UART.
     *
     * @param format printf-style format string.
     * @param ... Arguments for formatting.
     * @return Number of characters written to the UART.
     *
     * This function uses an internal buffer and vsnprintf
     * to format the string, then sends it via the send() method.
     */
    int print(const char* format, ...);

private:
    /**
     * @brief Default constructor.
     *
     * Initializes UART1, configures the GPIO pins, clocks,
     * and sets up registers for transmitting and receiving data.
     */
    Serial();

    /**
     * @brief Send a buffer of characters via UART.
     *
     * @param p_buf Pointer to the buffer containing the data.
     * @param size Number of characters to send.
     *
     * Uses putchar() to send each character individually.
     */
    void send(const char* p_buf, const int size);

    /**
     * @brief Send a single character via UART.
     *
     * @param c Character to transmit.
     *
     * Waits until the transmit register is empty,
     * then writes the character to TDR.
     */
    void putchar(const char c);

    /// Deleted copy constructor to prevent copying.
    Serial(const Serial&) = delete;

    /// Deleted copy assignment operator to prevent copying.
    Serial& operator=(const Serial&) = delete;

    /// Internal buffer for the formatted string.
    char buffer[buffer_size];
};

} // namespace serial

#endif // SERIAL_H