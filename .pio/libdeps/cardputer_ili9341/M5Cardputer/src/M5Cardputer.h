/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef M5CARDPUTER_H
#define M5CARDPUTER_H

#include "utility/Keyboard/Keyboard.h"
#include <M5Unified.h>

namespace m5 {

class M5_CARDPUTER {
public:
    void begin(bool enableKeyboard = true);
    void begin(m5::M5Unified::config_t cfg, bool enableKeyboard = true);

    M5GFX &Display         = M5.Display;
    M5GFX &Lcd             = Display;
    Power_Class &Power     = M5.Power;
    Speaker_Class &Speaker = M5.Speaker;
    Mic_Class &Mic         = M5.Mic;
    Button_Class &BtnA     = M5.getButton(0);

    Keyboard_Class Keyboard = Keyboard_Class();

    /// for internal I2C device
    I2C_Class &In_I2C = m5::In_I2C;

    /// for external I2C device (Port.A)
    I2C_Class &Ex_I2C = m5::Ex_I2C;

    void update(void);

private:
    /* data */
    bool _enableKeyboard;
};

}  // namespace m5

extern m5::M5_CARDPUTER M5Cardputer;

#endif