/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include "KeyboardReader.h"
#include "../../Adafruit_TCA8418/Adafruit_TCA8418.h"
#include <Arduino.h>
#include <vector>
#include <memory>

/**
 * @brief TCA8418 I2C keyboard reader implementation
 */
class TCA8418KeyboardReader : public KeyboardReader {
public:
    TCA8418KeyboardReader(int interrupt_pin = -1);
    virtual ~TCA8418KeyboardReader() = default;

    void begin() override;
    void update() override;

private:
    struct KeyEventRaw_t {
        bool state  = false;
        uint8_t row = 0;
        uint8_t col = 0;
    };

    std::unique_ptr<Adafruit_TCA8418> _tca8418;
    volatile bool _isr_flag;
    int _interrupt_pin;
    KeyEventRaw_t _key_event_raw_buffer;

    static void IRAM_ATTR gpio_isr_handler(void* arg);
    KeyEventRaw_t get_key_event_raw(const uint8_t& eventRaw);
    void remap(KeyEventRaw_t& eventRaw);
    void update_key_list(const KeyEventRaw_t& eventRaw);
};
