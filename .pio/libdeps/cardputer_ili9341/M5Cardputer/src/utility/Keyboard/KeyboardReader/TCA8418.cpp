/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "TCA8418.h"
#include "../../common.h"
#include "../../Adafruit_TCA8418/Adafruit_TCA8418_registers.h"
#include <Arduino.h>
#include <M5Unified.h>

// Default interrupt pin for M5Cardputer ADV
#define DEFAULT_TCA8418_INT_PIN 11

TCA8418KeyboardReader::TCA8418KeyboardReader(int interrupt_pin) : _isr_flag(false), _interrupt_pin(interrupt_pin)
{
    if (_interrupt_pin < 0) {
        _interrupt_pin = DEFAULT_TCA8418_INT_PIN;
    }
}

void IRAM_ATTR TCA8418KeyboardReader::gpio_isr_handler(void* arg)
{
    TCA8418KeyboardReader* reader = static_cast<TCA8418KeyboardReader*>(arg);
    reader->_isr_flag             = true;
}

void TCA8418KeyboardReader::begin()
{
    // Initialize TCA8418
    _tca8418 = std::make_unique<Adafruit_TCA8418>();

    if (!_tca8418->begin()) {
        printf("[error] TCA8418KeyboardReader: Failed to initialize TCA8418\n");
        return;
    }

    // Setup
    _tca8418->matrix(7, 8);
    _tca8418->flush();

    // Setup interrupt pin
    if (_interrupt_pin >= 0) {
        pinMode(_interrupt_pin, INPUT);
        attachInterruptArg(digitalPinToInterrupt(_interrupt_pin), gpio_isr_handler, this, CHANGE);
    }

    // Enable interrupts
    _tca8418->enableInterrupts();
}

void TCA8418KeyboardReader::update()
{
    if (!_isr_flag) {
        return;
    }

    _key_event_raw_buffer = get_key_event_raw(_tca8418->getEvent());

    //  try to clear the IRQ flag
    //  if there are pending events it is not cleared
    _tca8418->writeRegister8(TCA8418_REG_INT_STAT, 1);
    int intstat = _tca8418->readRegister8(TCA8418_REG_INT_STAT);
    if ((intstat & 0x01) == 0) {
        _isr_flag = false;
    }

    remap(_key_event_raw_buffer);
    // printf("key event raw: (%d, %d): %d\n", _key_event_raw_buffer.row, _key_event_raw_buffer.col,
    //        _key_event_raw_buffer.state);

    update_key_list(_key_event_raw_buffer);
}

TCA8418KeyboardReader::KeyEventRaw_t TCA8418KeyboardReader::get_key_event_raw(const uint8_t& eventRaw)
{
    KeyEventRaw_t ret;
    ret.state       = eventRaw & 0x80;
    uint16_t buffer = eventRaw;
    buffer &= 0x7F;
    buffer--;
    ret.row = buffer / 10;
    ret.col = buffer % 10;
    return ret;
}

// Remap to the same as cardputer
void TCA8418KeyboardReader::remap(KeyEventRaw_t& key)
{
    // Col
    uint8_t col = 0;
    col         = key.row * 2;
    if (key.col > 3) col++;

    // Row
    uint8_t row = 0;
    row         = (key.col + 4) % 4;

    key.row = row;
    key.col = col;
}

void TCA8418KeyboardReader::update_key_list(const KeyEventRaw_t& eventRaw)
{
    Point2D_t point;
    point.x = eventRaw.col;
    point.y = eventRaw.row;

    // Add or remove the key from the list
    if (eventRaw.state) {
        auto it = std::find(_key_list.begin(), _key_list.end(), point);
        if (it == _key_list.end()) {
            _key_list.push_back(point);
        }
    } else {
        auto it = std::find(_key_list.begin(), _key_list.end(), point);
        if (it != _key_list.end()) {
            _key_list.erase(it);
        }
    }
}
