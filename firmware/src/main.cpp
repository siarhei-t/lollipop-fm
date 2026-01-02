/**
 * @file main.cpp
 *
 * @brief tbd
 *
 *
 */

/* Includes ----------------------------------------------------------------- */

#include "FreeRTOS.h"
#include "led.hpp"
#include "system_stm32c0xx.h"
#include "task.h"

/* Public functions --------------------------------------------------------- */

int main(void)
{
    LedControl& led_control = LedControl::instance();
    led_control.start();
    vTaskStartScheduler();
    for (;;)
    {
    }
}

/* ------------------------------ end of file ------------------------------- */
