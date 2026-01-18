/**
 * @file buzzer.cpp
 *
 * @brief
 *
 */

#include "buzzer.hpp"
#include <stm32c011xx.h>

namespace bz
{

enum class Note : uint8_t
{
    C6,
    D6,
    E6,
    F6,
    G6,
    A6,
    B6,
    C7,
    D7,
    E7,
    F7,
    G7,
    A7,
    B7,
    None
};

constexpr uint16_t getFrequency(Note n)
{
    switch (n)
    {
        case Note::C6:
            return 1047;
        case Note::D6:
            return 1175;
        case Note::E6:
            return 1319;
        case Note::F6:
            return 1397;
        case Note::G6:
            return 1568;
        case Note::A6:
            return 1760;
        case Note::B6:
            return 1976;

        case Note::C7:
            return 2093;
        case Note::D7:
            return 2349;
        case Note::E7:
            return 2637;
        case Note::F7:
            return 2794;
        case Note::G7:
            return 3136;
        case Note::A7:
            return 3520;
        case Note::B7:
            return 3951;

        case Note::None:
            return 0;
    }
    return 0;
}

enum class BuzzerCmd : uint8_t
{
    PlayOnce,
    PlayLoop,
    Stop
};

struct Tone
{
    Note note;
    std::uint16_t duration_ms;
};

struct BuzzerMessage
{
    BuzzerCmd cmd;
    const Tone* melody;
    size_t length;
};

Buzzer& Buzzer::instance()
{
    static Buzzer bz;
    return bz;
}

Buzzer::Buzzer()
{

    RCC->IOPENR |= RCC_IOPENR_GPIOBEN;
    RCC->APBENR1 |= RCC_APBENR1_TIM3EN;

    // PB7 AF3 TIM3_CH2
    GPIOB->MODER &= ~GPIO_MODER_MODE7;
    GPIOB->MODER |= GPIO_MODER_MODE7_1;
    GPIOB->AFR[0] &= ~GPIO_AFRL_AFSEL7_Msk;
    GPIOB->AFR[0] |= (3U << GPIO_AFRL_AFSEL7_Pos);
    // TIM3 Channel 4 PWM mode 1
    TIM3->CCMR2 &= ~(TIM_CCMR2_OC4M_Msk | TIM_CCMR2_OC4PE);
    TIM3->CCMR2 |= (6U << TIM_CCMR2_OC4M_Pos) | TIM_CCMR2_OC4PE;
    TIM3->PSC = 0;

    uint32_t period = SystemCoreClock / default_frequency_hz - 1;
    TIM3->ARR = period;
    TIM3->CCR4 = (period + 1) / 2; // 50%
    TIM3->CR1 |= TIM_CR1_ARPE;
    TIM3->EGR = TIM_EGR_UG;
}

void Buzzer::start()
{
    TIM3->CCER |= TIM_CCER_CC4E;
    TIM3->CR1 |= TIM_CR1_CEN;
}

void Buzzer::stop()
{
    TIM3->CCER &= ~TIM_CCER_CC4E;
    TIM3->CR1 &= ~TIM_CR1_CEN;
}

void Buzzer::setFrequency(const std::uint16_t frequency_hz)
{
    uint32_t timer_clk = SystemCoreClock;
    uint32_t period = timer_clk / frequency_hz - 1;

    TIM3->ARR = period;
    TIM3->CCR2 = (period + 1) / 2;
}

void Buzzer::buzzerTask(void* pvParameters)
{
    (void)(pvParameters);

    for (;;)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
        /*
        xQueueReceive(buzzerQueue, &msg, portMAX_DELAY);

        if (msg.cmd == BuzzerCmd::Stop) {
            buzzer.stop();
            continue;
        }

        const Tone* melody = msg.melody;
        size_t len = msg.length;

        do {
            for (size_t i = 0; i < len; ++i) {


                if (xQueueReceive(buzzerQueue, &msg, 0) == pdTRUE) {
                    buzzer.stop();
                    break;
                }

                if (melody[i].note == Note::Silence) {
                    buzzer.stop();
                } else {
                    buzzer.setFrequency(noteFrequency(melody[i].note));
                    buzzer.start();
                }

                vTaskDelay(pdMS_TO_TICKS(melody[i].duration_ms));
            }

        } while (msg.cmd == BuzzerCmd::PlayLoop);

        buzzer.stop();*/
    }
}

void Buzzer::init() { task_handle = xTaskCreateStatic(&Buzzer::buzzerTask, "buzzer control", stack_size, nullptr, task_priority, stack, &task_buffer); }

} // namespace bz
