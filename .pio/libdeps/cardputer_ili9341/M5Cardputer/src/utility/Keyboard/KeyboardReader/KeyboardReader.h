/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <vector>

struct Point2D_t {
    int x = 0;
    int y = 0;

    bool operator==(const Point2D_t& other) const
    {
        return x == other.x && y == other.y;
    }
};

/**
 * @brief Abstract interface for keyboard reading implementations
 */
class KeyboardReader {
public:
    virtual ~KeyboardReader() = default;

    /**
     * @brief Initialize the keyboard reader
     */
    virtual void begin()
    {
    }

    /**
     * @brief Update the keyboard state and key list
     */
    virtual void update()
    {
    }

    /**
     * @brief Get the current key list
     * @return Reference to the key list
     */
    inline const std::vector<Point2D_t>& keyList() const
    {
        return _key_list;
    }

protected:
    std::vector<Point2D_t> _key_list;
};
