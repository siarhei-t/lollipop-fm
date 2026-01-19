/**
 * @file buzzer.hpp
 *
 * @brief
 *
 */

#ifndef BUZZER_H
#define BUZZER_H

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "queue.h"
#include "task.h"
#include <cstdint>

namespace bz
{

constexpr size_t queue_length = 2;
constexpr int stack_size = configMINIMAL_STACK_SIZE;
constexpr int task_priority = configMAX_PRIORITIES - 2;

enum class Note : std::uint8_t
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

enum class PlayType : std::uint8_t
{
    Once,
    Loop
};

struct Tone
{
    Note note = Note::None;
    std::uint16_t duration_ms = 0;
};

struct Sound
{
    Sound() = default;
    Sound(const PlayType type, const Tone* melody, const size_t length) : type(type), melody(melody), length(length) {}
    const PlayType type = PlayType::Once;
    const Tone* melody = nullptr;
    const size_t length = 0;
};

class Buzzer
{
public:
    static Buzzer& instance();
    void init();
    void playFerrite();
    void playNoneFerrite();
    void stopPlaying();
    static void buzzerTask(void* pvParameters);

private:
    Buzzer(const Buzzer&) = delete;
    Buzzer& operator=(const Buzzer&) = delete;
    Buzzer();
    void start();
    void stop();
    void setFrequency(const std::uint16_t frequency_hz);
    std::uint16_t frequency_hz = 0;
    QueueHandle_t queue;
    StackType_t stack[stack_size];
    StaticTask_t task_buffer;
    StaticQueue_t queue_struct;
    std::uint8_t queue_storage[queue_length * sizeof(Sound)];
    TaskHandle_t task_handle = nullptr;
};

} // namespace bz

#endif // BUZZER_H