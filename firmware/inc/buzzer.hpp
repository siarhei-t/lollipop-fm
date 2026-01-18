/**
 * @file buzzer.hpp
 *
 * @brief
 *
 */

#ifndef BUZZER_H
#define BUZZER_H

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include <cstdint>

namespace bz
{

constexpr int stack_size = configMINIMAL_STACK_SIZE;
constexpr int task_priority = configMAX_PRIORITIES - 3;
constexpr std::uint32_t default_frequency_hz = 1000;

class Buzzer
{
public:
    static Buzzer& instance();
    void init();
    void start();
    void stop();
    void setFrequency(const std::uint16_t frequency_hz);
    static void buzzerTask(void* pvParameters);

private:
    Buzzer();
    /// Deleted copy constructor to prevent copying.
    Buzzer(const Buzzer&) = delete;

    /// Deleted copy assignment operator to prevent copying.
    Buzzer& operator=(const Buzzer&) = delete;
    std::uint16_t frequency_hz = default_frequency_hz;
    QueueHandle_t queue;
    StackType_t stack[stack_size];

    StaticTask_t task_buffer;

    TaskHandle_t task_handle = nullptr;
};

} // namespace bz

#endif // BUZZER_H