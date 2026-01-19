/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include "KeyboardReader/KeyboardReader.h"
#include "Keyboard_def.h"
#include <Arduino.h>
#include <vector>
#include <memory>

struct KeyValue_t {
    const char value_first;
    const char value_second;
};

const KeyValue_t _key_value_map[4][14] = {{{'`', '~'},
                                           {'1', '!'},
                                           {'2', '@'},
                                           {'3', '#'},
                                           {'4', '$'},
                                           {'5', '%'},
                                           {'6', '^'},
                                           {'7', '&'},
                                           {'8', '*'},
                                           {'9', '('},
                                           {'0', ')'},
                                           {'-', '_'},
                                           {'=', '+'},
                                           {KEY_BACKSPACE, KEY_BACKSPACE}},
                                          {{KEY_TAB, KEY_TAB},
                                           {'q', 'Q'},
                                           {'w', 'W'},
                                           {'e', 'E'},
                                           {'r', 'R'},
                                           {'t', 'T'},
                                           {'y', 'Y'},
                                           {'u', 'U'},
                                           {'i', 'I'},
                                           {'o', 'O'},
                                           {'p', 'P'},
                                           {'[', '{'},
                                           {']', '}'},
                                           {'\\', '|'}},
                                          {{KEY_FN, KEY_FN},
                                           {KEY_LEFT_SHIFT, KEY_LEFT_SHIFT},
                                           {'a', 'A'},
                                           {'s', 'S'},
                                           {'d', 'D'},
                                           {'f', 'F'},
                                           {'g', 'G'},
                                           {'h', 'H'},
                                           {'j', 'J'},
                                           {'k', 'K'},
                                           {'l', 'L'},
                                           {';', ':'},
                                           {'\'', '\"'},
                                           {KEY_ENTER, KEY_ENTER}},
                                          {{KEY_LEFT_CTRL, KEY_LEFT_CTRL},
                                           {KEY_OPT, KEY_OPT},
                                           {KEY_LEFT_ALT, KEY_LEFT_ALT},
                                           {'z', 'Z'},
                                           {'x', 'X'},
                                           {'c', 'C'},
                                           {'v', 'V'},
                                           {'b', 'B'},
                                           {'n', 'N'},
                                           {'m', 'M'},
                                           {',', '<'},
                                           {'.', '>'},
                                           {'/', '?'},
                                           {' ', ' '}}};

class Keyboard_Class {
public:
    struct KeysState {
        bool tab          = false;
        bool fn           = false;
        bool shift        = false;
        bool ctrl         = false;
        bool opt          = false;
        bool alt          = false;
        bool del          = false;
        bool enter        = false;
        bool space        = false;
        uint8_t modifiers = 0;

        std::vector<char> word;
        std::vector<uint8_t> hid_keys;
        std::vector<uint8_t> modifier_keys;

        void reset()
        {
            tab       = false;
            fn        = false;
            shift     = false;
            ctrl      = false;
            opt       = false;
            alt       = false;
            del       = false;
            enter     = false;
            space     = false;
            modifiers = 0;
            word.clear();
            hid_keys.clear();
            modifier_keys.clear();
        }
    };

    Keyboard_Class() : _is_caps_locked(false)
    {
    }

    void begin();
    void begin(std::unique_ptr<KeyboardReader> reader);
    uint8_t getKey(Point2D_t keyCoor);

    void updateKeyList();
    inline const std::vector<Point2D_t>& keyList()
    {
        if (_keyboard_reader) {
            return _keyboard_reader->keyList();
        }
        static const std::vector<Point2D_t> empty_list;
        return empty_list;
    }

    inline KeyValue_t getKeyValue(const Point2D_t& keyCoor)
    {
        return _key_value_map[keyCoor.y][keyCoor.x];
    }

    uint8_t isPressed();
    bool isChange();
    bool isKeyPressed(char c);

    void updateKeysState();
    inline KeysState& keysState()
    {
        return _keys_state_buffer;
    }

    inline bool capslocked(void)
    {
        return _is_caps_locked;
    }
    inline void setCapsLocked(bool isLocked)
    {
        _is_caps_locked = isLocked;
    }

private:
    std::unique_ptr<KeyboardReader> _keyboard_reader;
    std::vector<Point2D_t> _key_pos_print_keys;     // only text: eg A,B,C
    std::vector<Point2D_t> _key_pos_hid_keys;       // print key + space, enter, del
    std::vector<Point2D_t> _key_pos_modifier_keys;  // modifier key: eg shift, ctrl, alt
    KeysState _keys_state_buffer;
    bool _is_caps_locked;
    uint8_t _last_key_size;
};
