/**
 * @file io.hpp
 *
 * @brief
 *
 */

#ifndef IO_H
#define IO_H

// #include <cstdint>

namespace io
{

class DevicePorts
{

public:
    static DevicePorts& instance();
    void oscSetState(const bool state);

private:
    DevicePorts();

    /// Deleted copy constructor to prevent copying.
    DevicePorts(const DevicePorts&) = delete;

    /// Deleted copy assignment operator to prevent copying.
    DevicePorts& operator=(const DevicePorts&) = delete;
};

} // namespace io

#endif // IO_H