/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "ReplView.h"
#include <M5Cardputer.h>

LGFX_Sprite *canvas;
ReplView repl_view;
long the_number    = 2333;
bool is_player_win = false;

bool string_to_number(const char *str, long *out)
{
    if (str == NULL || *str == '\0') {
        return false;
    }

    char *end;
    long value = strtol(str, &end, 10);

    if (*end != '\0') {
        return false;
    }

    if (value < -999 || value > 999) {
        return false;
    }

    *out = value;
    return true;
}

void handle_command(const std::string &command)
{
    /* If player win, show the message */
    if (is_player_win) {
        repl_view.showMessage(command, TFT_CYAN);
        return;
    }

    /* Convert input to number */
    long number = 0;
    if (!string_to_number(command.c_str(), &number)) {
        repl_view.showMessage("Invaild input :(", TFT_RED);
        return;
    }

    /* Compare */
    if (number == the_number) {
        repl_view.showMessage("Correct! :)", TFT_GREENYELLOW);
        is_player_win = true;
    } else if (number > the_number) {
        repl_view.showMessage("Too high!", TFT_CYAN);
    } else {
        repl_view.showMessage("Too low!", TFT_CYAN);
    }
}

void setup()
{
    M5Cardputer.begin();

    /* Create a full screen canvas for view */
    canvas = new LGFX_Sprite(&M5Cardputer.Display);
    canvas->createSprite(M5Cardputer.Display.width(), M5Cardputer.Display.height());

    /* Init REPL view, setup callbacks */
    repl_view.onCommand    = handle_command;
    repl_view.onRenderTips = []() {
        canvas->setTextColor(TFT_CYAN);
        canvas->println("Guess the number");
        canvas->println("The number is between\n-999 and 999 :D");
    };
    repl_view.init(canvas);

    /* Get a random number */
    the_number = random(-999, 999);
}

void loop()
{
    /* Keep updating */
    M5Cardputer.update();
    repl_view.update();
}
