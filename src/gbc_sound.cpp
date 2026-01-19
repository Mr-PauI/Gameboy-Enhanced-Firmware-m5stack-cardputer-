#include "gbc_sound.h"

#include <Arduino.h>
#include <M5Cardputer.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static constexpr int kChannel = 0;

// ================== STATO AUDIO ==================

static bool   s_inited      = false;
int    gbc_sampleRate = 32768; // Default Walnut rate

static TaskHandle_t s_audioTask  = nullptr;
static volatile bool s_running   = false;

// Buffer circolare da 4KB (2048 campioni a 16bit)
static constexpr int kRingSamples = 4096; 
static int16_t* s_ring            = nullptr;
static int      s_ringSize        = 0;
static int      s_ringRead        = 0;
static int      s_ringWrite       = 0;
static int      s_ringCount       = 0;

// Protezione thread-safe
static portMUX_TYPE s_ringMux = portMUX_INITIALIZER_UNLOCKED;

// ================== TASK AUDIO ==================

static void gbc_audio_task(void* arg)
{
    static const int kChunk = 512;
    int16_t local[kChunk];

    while (s_running) {
        // Se il buffer è vuoto, aspetta un po'
        if (s_ringCount < kChunk) {
            vTaskDelay(1);
            continue;
        }

        // Legge un blocco dal ring buffer in modo thread-safe
        int toRead = 0;
        portENTER_CRITICAL(&s_ringMux);
        if (s_ringCount > 0) {
            toRead = (s_ringCount < kChunk) ? s_ringCount : kChunk;

            for (int i = 0; i < toRead; ++i) {
                local[i] = s_ring[s_ringRead];
                s_ringRead++;
                if (s_ringRead >= s_ringSize) s_ringRead = 0;
            }

            s_ringCount -= toRead;
        }
        portEXIT_CRITICAL(&s_ringMux);

        if (toRead <= 0) {
            continue;
        }

        // Invia allo speaker M5 (non bloccante se possibile)
        // M5Unified gestisce il DMA interno
        while (M5Cardputer.Speaker.isPlaying(kChannel) > 1) {
             // Se lo speaker è pieno, aspettiamo un attimo per evitare overflow
             vTaskDelay(1);
        }

        M5Cardputer.Speaker.playRaw(
            local,
            (size_t)toRead,
            (uint32_t)gbc_sampleRate,
            false, 1, kChannel, false
        );
    }

    vTaskDelete(nullptr);
}


// ================== API ==================

extern "C" void gbc_sound_init(int sample_rate)
{
    if (s_inited) return;

    if (sample_rate > 0) {
        gbc_sampleRate = sample_rate;
    }

    auto cfg = M5Cardputer.Speaker.config();
    cfg.sample_rate       = gbc_sampleRate;
    cfg.stereo            = false; // Walnut esce stereo, ma qui mixiamo a mono per performance
    cfg.dma_buf_len       = 256;
    cfg.dma_buf_count     = 8;
    cfg.task_priority     = 2; // Priorità media
    cfg.task_pinned_core  = 0; // Core 0 per l'audio di sistema
    M5Cardputer.Speaker.config(cfg);

    if (!M5Cardputer.Speaker.isRunning()) {
        M5Cardputer.Speaker.begin();
    }

    M5Cardputer.Speaker.setVolume(180);
    M5Cardputer.Speaker.stop(kChannel);

    // Allocazione Ring Buffer
    if (!s_ring) {
        s_ringSize  = kRingSamples;
        s_ring      = (int16_t*)malloc(s_ringSize * sizeof(int16_t));
        s_ringRead  = 0;
        s_ringWrite = 0;
        s_ringCount = 0;
    }

    s_running = true;

    if (!s_audioTask && s_ring) {
        BaseType_t ok = xTaskCreatePinnedToCore(
            gbc_audio_task,
            "gbc_audio",
            4096,
            nullptr,
            5,      // Priority alta per evitare stuttering
            &s_audioTask,
            0       // Core 0
        );
        if (ok != pdPASS) {
            Serial.println("[GBC][AUDIO] task create failed");
            s_audioTask = nullptr;
            s_running   = false;
        }
    }

    s_inited = (s_ring != nullptr);
    Serial.printf("[GBC][AUDIO] init: rate=%d mono, ring=%d\n", gbc_sampleRate, s_ringSize);
}

extern "C" void gbc_sound_set_volume(uint8_t vol)
{
    M5Cardputer.Speaker.setVolume(vol);
}

extern "C" void gbc_sound_shutdown(void)
{
    if (!s_inited) return;

    s_running = false;

    if (s_audioTask) {
        vTaskDelay(10);
        s_audioTask = nullptr;
    }

    M5Cardputer.Speaker.stop(kChannel);

    if (s_ring) {
        free(s_ring);
        s_ring = nullptr;
    }
    s_ringSize  = 0;
    s_ringRead  = 0;
    s_ringWrite = 0;
    s_ringCount = 0;

    s_inited = false;
}

extern "C" void gbc_sound_submit(const int16_t* samples, size_t sample_count)
{
    if (!s_inited || !samples || sample_count == 0 || !s_ring) {
        return;
    }

    portENTER_CRITICAL(&s_ringMux);

    int freeSpace = s_ringSize - s_ringCount;
    
    // Se non c'è spazio, droppiamo i campioni (meglio perdere audio che bloccare l'emulatore)
    if (freeSpace < (int)sample_count) {
         portEXIT_CRITICAL(&s_ringMux);
         return; 
    }

    int toWrite = (int)sample_count;

    for (int i = 0; i < toWrite; ++i) {
        s_ring[s_ringWrite] = samples[i];
        s_ringWrite++;
        if (s_ringWrite >= s_ringSize) s_ringWrite = 0;
    }
    s_ringCount += toWrite;

    portEXIT_CRITICAL(&s_ringMux);
}