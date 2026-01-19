/**
 * minigb_apu is released under the terms listed within the LICENSE file.
 *
 * minigb_apu emulates the audio processing unit (APU) of the Game Boy. This
 * project is based on MiniGBS by Alex Baines: https://github.com/baines/MiniGBS
 */

#pragma once

#include <stdint.h>

// FORCE AUDIO FORMAT HERE TO FIX COMPILATION ERRORS
#ifndef MINIGB_APU_AUDIO_FORMAT_S16SYS
#define MINIGB_APU_AUDIO_FORMAT_S16SYS
#endif

#define AUDIO_SAMPLE_RATE   32768 // was 32768

/**
 * DMG_CLOCK_FREQ is 4194304Hz.
 * SCREEN_REFRESH_CYCLES is 70224 cycles.
 * VERTICAL_SYNC is 59.7275Hz.
 */
#define DMG_CLOCK_FREQ      4194304.0
#define SCREEN_REFRESH_CYCLES   70224.0
#define VERTICAL_SYNC       (DMG_CLOCK_FREQ/SCREEN_REFRESH_CYCLES)

/**
 * AUDIO_SAMPLES is the number of samples to produce per video frame.
 */
#define AUDIO_SAMPLES       ((unsigned)(AUDIO_SAMPLE_RATE / VERTICAL_SYNC))

#if defined(MINIGB_APU_AUDIO_FORMAT_S16SYS)
    typedef int16_t audio_sample_t;
#elif defined(MINIGB_APU_AUDIO_FORMAT_FLOAT)
    typedef float audio_sample_t;
#else
    #error MiniGB APU: Invalid or unsupported audio format selected
#endif

struct minigb_apu_ctx;

/**
 * Initialize the APU context.
 */
void minigb_apu_audio_init(struct minigb_apu_ctx *ctx);

/**
 * Read audio register at given address "addr".
 */
uint8_t minigb_apu_audio_read(struct minigb_apu_ctx *ctx,
    const uint16_t addr);

/**
 * Write "val" to audio register at given address "addr".
 */
void minigb_apu_audio_write(struct minigb_apu_ctx *ctx,
    const uint16_t addr, const uint8_t val);

/**
 * Fill buffer "stream" with "AUDIO_SAMPLES" stereo samples.
 * stream size must be at least AUDIO_SAMPLES * 2 * sizeof(audio_sample_t).
 */
void minigb_apu_audio_callback(struct minigb_apu_ctx *ctx,
    audio_sample_t *stream);

/**
 * Internal context structure. 
 * Defined here so we can allocate it statically in main.cpp if needed.
 */
struct chan_len_ctr {
    uint8_t load;
    unsigned enabled : 1;
    uint32_t counter;
    uint32_t inc;
};

struct chan_vol_env {
    uint8_t step;
    unsigned up : 1;
    uint32_t counter;
    uint32_t inc;
};

struct chan_freq_sweep {
    uint16_t freq;
    uint8_t rate;
    uint8_t shift;
    unsigned up : 1;
    uint32_t counter;
    uint32_t inc;
};

struct chan {
    unsigned enabled : 1;
    unsigned powered : 1;
    unsigned on_left : 1;
    unsigned on_right : 1;
    unsigned muted : 1;

    uint8_t volume;
    uint8_t volume_init;

    uint16_t freq;
    uint32_t freq_counter;
    uint32_t freq_inc;

    int_fast16_t val;

    struct chan_len_ctr    len;
    struct chan_vol_env    env;
    struct chan_freq_sweep sweep;

    union {
        struct {
            uint8_t duty;
            uint8_t duty_counter;
        } square;
        struct {
            uint16_t lfsr_reg;
            uint8_t  lfsr_wide;
            uint8_t  lfsr_div;
        } noise;
        struct {
            uint8_t sample;
        } wave;
    };
};

struct minigb_apu_ctx {
    uint8_t audio_mem[0xFF3F - 0xFF10 + 1];
    struct chan chans[4];
    int32_t vol_l, vol_r;
};