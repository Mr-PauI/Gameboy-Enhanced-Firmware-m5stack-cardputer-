/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include "KeyboardReader.h"
#include <Arduino.h>
#include <vector>

struct Chart_t {
    uint8_t value;
    uint8_t x_1;
    uint8_t x_2;
};

/**
 * @brief IO Matrix keyboard reader implementation
 */
class IOMatrixKeyboardReader : public KeyboardReader {
public:
    IOMatrixKeyboardReader()          = default;
    virtual ~IOMatrixKeyboardReader() = default;

    void begin() override;
    void update() override;

private:
    const std::vector<int> output_list = {8, 9, 11};
    const std::vector<int> input_list  = {13, 15, 3, 4, 5, 6, 7};

    const Chart_t X_map_chart[7] = {{1, 0, 1}, {2, 2, 3}, {4, 4, 5}, {8, 6, 7}, {16, 8, 9}, {32, 10, 11}, {64, 12, 13}};

    void set_output(const std::vector<int>& pinList, uint8_t output);
    uint8_t get_input(const std::vector<int>& pinList);
};
