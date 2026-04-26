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

class DevicePorts
{

public:
    static DevicePorts& instance();

    void oscSetState(const bool state);
    bool checkButtonEvent();
    void irq();

private:
    DevicePorts();

    DevicePorts(const DevicePorts&) = delete;
    DevicePorts& operator=(const DevicePorts&) = delete;
    volatile bool button_pressed;
};

} // namespace io

#endif // IO_H