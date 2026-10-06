/**
 * @file fm.cpp
 *
 * @brief
 *
 */

#include "fm.hpp"
#include "config.hpp"
#include <stm32c011xx.h>

namespace fm
{

FrequencyMeter& FrequencyMeter::instance()
{
    static FrequencyMeter fm;
    return fm;
}

FrequencyMeter::FrequencyMeter()
{
    RCC->APBENR2 |= RCC_APBENR2_TIM1EN;
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
    // LOLLIPOP-FM rev. 1.0 board
    // PA0 AF5 = TIM1_CH1
    GPIOA->MODER = (GPIOA->MODER & ~GPIO_MODER_MODE0) | GPIO_MODER_MODE0_1;
    GPIOA->AFR[0] = (GPIOA->AFR[0] & ~GPIO_AFRL_AFSEL0) | (5U << GPIO_AFRL_AFSEL0_Pos);

    // TIM1 without prescaler, max count limiter
    TIM1->PSC = 0;
    TIM1->ARR = 0xFFFF;

    // CH1 — capture mode: CC1S=01 ( input TI1), IC1PSC=11 (every 8 front)
    // CH2 — pulse WDT, interrupt in case of out of range frequency

    TIM1->CCMR1 = (1U << TIM_CCMR1_CC1S_Pos) | (3U << TIM_CCMR1_IC1PSC_Pos);

    TIM1->CCER = TIM_CCER_CC1E; // rising edge
    TIM1->EGR = TIM_EGR_UG;
    TIM1->SR = 0;
    TIM1->DIER = TIM_DIER_CC1IE | TIM_DIER_CC2IE;
}

void FrequencyMeter::setPeriods(std::uint16_t periods)
{
    periods &= ~(cfg::edges_per_capture - 1); // round to 8
    if ((periods >= cfg::periods_min) && (periods <= cfg::periods_max))
    {
        new_periods = periods;
    }
}

void FrequencyMeter::start()
{
    have_last = false;
    ready = false;
    res_fault = false;
    fault = false;
    streak = 0;
    startWindow();

    TIM1->CNT = 0;
    TIM1->CCR2 = cfg::timeout_ticks;
    TIM1->SR = 0;
    TIM1->CR1 |= TIM_CR1_CEN;
    NVIC_EnableIRQ(TIM1_CC_IRQn);
}

void FrequencyMeter::stop()
{
    NVIC_DisableIRQ(TIM1_CC_IRQn);
    TIM1->CR1 &= ~TIM_CR1_CEN;
}

std::uint32_t FrequencyMeter::getValue()
{
    const std::uint32_t primask = __get_PRIMASK();
    __disable_irq();
    const std::uint32_t ticks = res_ticks;
    value_periods = res_periods;
    fault = res_fault;
    ready = false;
    __set_PRIMASK(primask);
    return ticks;
}

void FrequencyMeter::irq()
{
    const std::uint32_t sr = TIM1->SR;

    if (sr & TIM_SR_CC1OF)
    {
        TIM1->SR = ~TIM_SR_CC1OF;
        have_last = false;
    }

    if (sr & TIM_SR_CC1IF)
    {
        onCapture(TIM1->CCR1); // reading of CCR1 reset CC1IF flag
    }
    else if (sr & TIM_SR_CC2IF)
    {
        onTimeout();
    }
}

void FrequencyMeter::startWindow()
{
    if (new_periods != 0)
    {
        window_periods = new_periods;
        new_periods = 0;
    }
    window_scaled = std::uint64_t(window_periods) * cfg::timer_clock_hz;
    captures_left = window_periods >> cfg::capture_shift;
    win_ticks = 0;
}

void FrequencyMeter::onCapture(std::uint16_t t)
{
    // WDT reload
    TIM1->CCR2 = static_cast<std::uint16_t>(t + cfg::timeout_ticks);
    TIM1->SR = ~TIM_SR_CC2IF;

    const std::uint16_t delta = t - last_capture;
    const bool valid = have_last && delta < cfg::timeout_ticks;

    if (have_last && !valid)
    {
        publish(0); // timeout
    }

    last_capture = t;
    have_last = true;

    if (!valid)
    {
        startWindow();
        return;
    }

    win_ticks += delta;
    if (--captures_left == 0)
    {
        publish(win_ticks);
        startWindow();
    }
}

void FrequencyMeter::onTimeout()
{
    // reload timeout
    TIM1->CCR2 = static_cast<std::uint16_t>(TIM1->CCR2 + cfg::timeout_ticks);
    TIM1->SR = ~TIM_SR_CC2IF;
    have_last = false;
    publish(0);
}

void FrequencyMeter::publish(std::uint32_t ticks)
{
    // f = periods * f_timer / ticks
    // min_hz <= f <= max_hz  <=>  min_hz*ticks <= periods*f_timer <= max_hz*ticks
    const bool bad = (ticks == 0) || (window_scaled < (std::uint64_t(cfg::min_hz) * ticks)) || (window_scaled > (std::uint64_t(cfg::max_hz) * ticks));

    if (bad == res_fault)
    {
        streak = 0;
    }
    else if (++streak >= cfg::fault_debounce)
    {
        res_fault = bad;
        streak = 0;
    }

    res_ticks = ticks;
    res_periods = window_periods;
    ready = true;
}

} // namespace fm

extern "C" void TIM1_CC_IRQHandler(void) { fm::FrequencyMeter::instance().irq(); }
