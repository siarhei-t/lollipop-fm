/**
 * @file buzzer.cpp
 *
 * @brief
 *
 */

#include "buzzer.hpp"
#include "config.hpp"
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

    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
    RCC->APBENR2 |= RCC_APBENR2_TIM16EN;
    // LOLLIPOP-FM rev. 1.0 board
    // PA6 AF5
    GPIOA->MODER &= ~GPIO_MODER_MODE6;
    GPIOA->MODER |= GPIO_MODER_MODE6_1;
    GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL6_Msk;
    GPIOA->AFR[0] |= (5U << GPIO_AFRL_AFSEL6_Pos);

    // TIM16 Channel 1 PWM mode 1
    TIM16->CCMR1 &= ~(TIM_CCMR1_OC1M_Msk | TIM_CCMR1_OC1PE);
    TIM16->CCMR1 |= (6U << TIM_CCMR1_OC1M_Pos) | TIM_CCMR1_OC1PE;
    TIM16->PSC = 0;
    TIM16->CR1 |= TIM_CR1_ARPE;
    TIM16->EGR = TIM_EGR_UG;
    TIM16->BDTR |= TIM_BDTR_MOE;
}

void Buzzer::playFerrite()
{
    static const Sound sound = Sound(PlayType::Loop, ferrite, sizeof(ferrite) / sizeof(Tone));
    xQueueSend(queue, &sound, 0);
}

void Buzzer::playNoneFerrite()
{
    static const Sound sound = Sound(PlayType::Loop, non_ferrite, sizeof(non_ferrite) / sizeof(Tone));
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
    TIM16->CCER |= TIM_CCER_CC1E;
    TIM16->CR1 |= TIM_CR1_CEN;
}

void Buzzer::stop()
{
    TIM16->CCER &= ~TIM_CCER_CC1E;
    TIM16->CR1 &= ~TIM_CR1_CEN;
}

void Buzzer::setFrequency(const std::uint16_t frequency_hz)
{
    if (this->frequency_hz != frequency_hz)
    {
        this->frequency_hz = frequency_hz;
        std::uint32_t timer_clk = SystemCoreClock;
        std::uint32_t period = timer_clk / frequency_hz - 1;
        TIM16->ARR = period;
        TIM16->CCR1 = (period + 1) / 2;
    }
}

void Buzzer::buzzerTask(void* pvParameters)
{
    static Sound sound;

    (void)(pvParameters);
    for (;;)
    {
        xQueueReceive(instance().queue, &sound, portMAX_DELAY);
        if (sound.melody == nullptr)
        {
            instance().stop();
            vTaskDelay(pdMS_TO_TICKS(cfg::update_rate_ms));
            continue;
        }
        const Tone* melody = sound.melody;
        for (size_t i = 0; i < sound.length; ++i)
        {
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
        instance().stop();
    }
}

void Buzzer::init()
{
    task_handle = xTaskCreateStatic(&Buzzer::buzzerTask, "buzzer control", stack_size, nullptr, task_priority, stack, &task_buffer);
    queue = xQueueCreateStatic(queue_length, sizeof(Sound), queue_storage, &queue_struct);
}

} // namespace bz
