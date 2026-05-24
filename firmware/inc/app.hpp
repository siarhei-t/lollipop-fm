/**
 * @file app.hpp
 *
 * @brief
 *
 */

#ifndef APP_H
#define APP_H

#include "FreeRTOS.h"
#include "task.h"

namespace app
{

constexpr int task_priority = configMAX_PRIORITIES - 3;
constexpr int stack_size = 512;

class Application
{
public:
    static Application& instance();
    void start();
    static void appTask(void* pvParameters);

private:
    Application() {}
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    // FreeRTOS static task and queue variables
    StackType_t stack[stack_size];
    StaticTask_t task_buffer;
    TaskHandle_t task_handle = nullptr;
};

} // namespace app

#endif // APP_H