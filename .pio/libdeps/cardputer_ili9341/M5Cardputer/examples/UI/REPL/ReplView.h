/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <M5Cardputer.h>
#include <functional>
#include <string>

/**
 * @brief
 *
 */
class ReplView {
public:
    std::function<void()> onRenderTips;
    std::function<void(const std::string& command)> onCommand;
    bool autoClearPrompt = true;

    void init(LGFX_Sprite* canvas);
    void update();
    void clearScreen();
    void showMessage(const std::string& message, uint16_t color = TFT_WHITE);
    void showPrompt(const std::string& prompt_text = ">>> ");
    void setPromptText(const std::string& prompt);

    void setInputBuffer(const std::string& text);
    const std::string& getInputBuffer() const
    {
        return _input_buffer;
    }
    void clearInputBuffer();

protected:
    static constexpr uint32_t CURSOR_BLINK_PERIOD = 500;

    LGFX_Sprite* _canvas         = nullptr;
    uint32_t _cursor_update_time = 0;
    bool _cursor_state           = false;
    std::string _input_buffer;
    std::string _prompt_text = ">>> ";
    int _key_event_slot_id   = -1;

    void render_interface();
    void render_prompt();
    void handle_enter_key();
    void handle_backspace();
    void update_cursor();
    void update_keyboard_input();
};
