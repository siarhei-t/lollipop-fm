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

class Serial
{
public:
    static Serial& instance();
    int print(const char* format, ...);

private:
    Serial();
    void send(const char* p_buf, const int size);
    void putchar(const char c);

    Serial(const Serial&) = delete;
    Serial& operator=(const Serial&) = delete;

    char buffer[buffer_size];
};

} // namespace serial

#endif // SERIAL_H