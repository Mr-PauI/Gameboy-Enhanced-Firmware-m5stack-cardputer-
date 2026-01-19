/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "ReplView.h"

void ReplView::init(LGFX_Sprite* canvas)
{
    _canvas = canvas;
    render_interface();
}

void ReplView::update()
{
    update_keyboard_input();
    update_cursor();
}

void ReplView::clearScreen()
{
    _canvas->fillScreen(TFT_BLACK);
    _canvas->setCursor(0, 0);
    _canvas->pushSprite(0, 0);
}

void ReplView::showMessage(const std::string& message, uint16_t color)
{
    _canvas->setTextColor(color, TFT_BLACK);
    _canvas->println(message.c_str());
    _canvas->setTextColor(TFT_WHITE, TFT_BLACK);
    _canvas->pushSprite(0, 0);
}

void ReplView::showPrompt(const std::string& prompt_text)
{
    _prompt_text = prompt_text;
    render_prompt();
}

void ReplView::setPromptText(const std::string& prompt)
{
    _prompt_text = prompt;
}

void ReplView::setInputBuffer(const std::string& text)
{
    _input_buffer = text;
    render_prompt();
}

void ReplView::clearInputBuffer()
{
    _input_buffer.clear();
    render_prompt();
}

void ReplView::render_interface()
{
    _canvas->fillScreen(TFT_BLACK);
    _canvas->setFont(&fonts::efontCN_16);
    _canvas->setTextScroll(true);
    _canvas->setCursor(0, 0);
    _canvas->setTextSize(1);
    _canvas->setBaseColor(TFT_BLACK);
    _canvas->setTextColor(TFT_WHITE, TFT_BLACK);

    if (onRenderTips) {
        onRenderTips();
    }

    render_prompt();
}

void ReplView::render_prompt()
{
    // 显示输入和光标
    std::string display_text = _prompt_text + _input_buffer;
    if (_cursor_state) {
        display_text += "_";
    } else {
        display_text += " ";
    }

    // 获取当前光标位置
    int cursor_y = _canvas->getCursorY();

    // 清除当前行，确保光标被完全移除
    _canvas->fillRect(0, cursor_y, _canvas->width(), 15, TFT_BLACK);

    // 打印提示符和输入
    _canvas->setCursor(0, cursor_y);
    _canvas->setTextColor(TFT_WHITE, TFT_BLACK);
    _canvas->print(display_text.c_str());

    _canvas->pushSprite(0, 0);
}

void ReplView::handle_enter_key()
{
    // 清除当前输入行并重新绘制不带光标的版本
    int cursor_y = _canvas->getCursorY();
    _canvas->fillRect(0, cursor_y, _canvas->width(), 15, TFT_BLACK);
    _canvas->setCursor(0, cursor_y);
    _canvas->setTextColor(TFT_WHITE, TFT_BLACK);
    _canvas->print(_prompt_text.c_str());
    _canvas->print(_input_buffer.c_str());
    _canvas->println();  // 移动到下一行

    // 通过回调执行命令
    if (onCommand && !_input_buffer.empty()) {
        onCommand(_input_buffer);
    }

    // 重置输入缓冲区并显示新提示符
    if (autoClearPrompt) {
        _input_buffer.clear();
    }
    render_prompt();
}

void ReplView::handle_backspace()
{
    if (!_input_buffer.empty()) {
        _input_buffer.pop_back();

        // 清除更大区域确保旧光标被完全移除
        int cursor_y = _canvas->getCursorY();
        _canvas->fillRect(0, cursor_y, _canvas->width(), 15, TFT_BLACK);

        // 重新绘制提示行
        render_prompt();
    }
}

void ReplView::update_cursor()
{
    if (millis() - _cursor_update_time > CURSOR_BLINK_PERIOD) {
        _cursor_state       = !_cursor_state;
        _cursor_update_time = millis();
        render_prompt();
    }
}

void ReplView::update_keyboard_input()
{
    if (M5Cardputer.Keyboard.isChange()) {
        if (M5Cardputer.Keyboard.isPressed()) {
            auto& status = M5Cardputer.Keyboard.keysState();

            if (status.enter) {
                handle_enter_key();
                return;
            }

            if (status.del) {
                handle_backspace();
                return;
            }

            for (auto& c : status.word) {
                _input_buffer += c;
            }
            render_prompt();
        }
    }
}
