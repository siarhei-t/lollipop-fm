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

/**
 * @brief Priority of the main application task.
 *
 * The priority is set slightly below the maximum to allow
 * higher-priority system or real-time tasks to preempt it if needed.
 */
constexpr int task_priority = configMAX_PRIORITIES - 2;

/**
 * @brief Stack size of the application task.
 *
 * Size is specified in FreeRTOS stack words (not bytes).
 * The value is chosen to be large enough to safely handle
 * operations such as formatted printing (e.g. vsnprintf).
 */
constexpr int stack_size = 256;

/**
 * @brief Main application controller class.
 *
 * This class encapsulates the creation and execution of the main
 * application task in FreeRTOS. It follows the singleton pattern
 * to ensure that only one instance of the application exists.
 *
 * The task is created using the FreeRTOS static allocation API
 * to avoid dynamic memory usage at runtime.
 */
class Application
{
public:
    /**
     * @brief Get the singleton instance of the Application.
     *
     * @return Reference to the Application instance.
     */
    static Application& instance();

    /**
     * @brief Initialize and start the application.
     *
     * This method creates the main application task and performs
     * any required initialization before the scheduler is started.
     */
    void start();

    /**
     * @brief Main application task entry point.
     *
     * This function is executed in the context of a FreeRTOS task.
     * It must have C linkage-compatible signature and is typically
     * implemented as an infinite loop.
     *
     * @param pvParameters Pointer to task parameters (unused).
     */
    static void appTask(void* pvParameters);

private:
    /**
     * @brief Private constructor.
     *
     * The constructor is private to enforce the singleton pattern.
     */
    Application() {}

    /// Deleted copy constructor to prevent copying.
    Application(const Application&) = delete;

    /// Deleted copy assignment operator to prevent copying.
    Application& operator=(const Application&) = delete;

    /**
     * @brief Stack memory for the application task.
     *
     * The stack is statically allocated and passed to
     * xTaskCreateStatic().
     */
    StackType_t stack[stack_size];

    /**
     * @brief Static task control block.
     *
     * Holds the FreeRTOS internal state of the task created
     * using static allocation.
     */
    StaticTask_t task_buffer;

    /**
     * @brief Handle of the application task.
     *
     * Used to reference the task after creation.
     */
    TaskHandle_t task_handle = nullptr;
};

} // namespace app

#endif // APP_H