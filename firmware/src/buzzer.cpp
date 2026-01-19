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

constexpr Tone ferrite[] = {{Note::F7, 200}, {Note::None, 200}};
constexpr Tone non_ferrite[] = {{Note::G7, 100}, {Note::None, 100}};

constexpr std::uint16_t getFrequency(Note n)
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
    TIM3->CR1 |= TIM_CR1_ARPE;
    TIM3->EGR = TIM_EGR_UG;
}

void Buzzer::playFerrite()
{
    Sound sound = Sound(PlayType::Loop, ferrite, sizeof(ferrite) / sizeof(Tone));
    xQueueSend(queue, &sound, 0);
}

void Buzzer::playNoneFerrite()
{
    Sound sound = Sound(PlayType::Loop, non_ferrite, sizeof(non_ferrite) / sizeof(Tone));
    xQueueSend(queue, &sound, 0);
}

void Buzzer::stopPlaying()
{
    Sound sound;
    frequency_hz = 0;
    xQueueSend(queue, &sound, 0);
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
    if (this->frequency_hz != frequency_hz)
    {
        this->frequency_hz = frequency_hz;
        uint32_t timer_clk = SystemCoreClock;
        uint32_t period = timer_clk / frequency_hz - 1;

        TIM3->ARR = period;
        TIM3->CCR4 = (period + 1) / 2;
    }
}

void Buzzer::buzzerTask(void* pvParameters)
{
    (void)(pvParameters);
    for (;;)
    {
        Sound sound;
        xQueueReceive(instance().queue, &sound, portMAX_DELAY);
        if (sound.melody == nullptr)
        {
            instance().stop();
            continue;
        }

        const Tone* melody = sound.melody;
        size_t len = sound.length;
        do
        {
            for (size_t i = 0; i < len; ++i)
            {
                if (uxQueueMessagesWaiting(instance().queue) > 0)
                {
                    instance().stop();
                    break;
                }

                if (melody[i].note == Note::None)
                {
                    instance().stop();
                }
                else
                {
                    instance().setFrequency(getFrequency(melody[i].note));
                    instance().start();
                }
                vTaskDelay(pdMS_TO_TICKS(melody[i].duration_ms));
            }

        } while (sound.type == PlayType::Loop);
        instance().stop();
    }
}

void Buzzer::init()
{
    task_handle = xTaskCreateStatic(&Buzzer::buzzerTask, "buzzer control", stack_size, nullptr, task_priority, stack, &task_buffer);
    queue = xQueueCreateStatic(queue_length, sizeof(Sound), queue_storage, &queue_struct);
}

} // namespace bz
