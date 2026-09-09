/*
 * Copyright (C) 2025-2026 Guy Wilson
 *
 * This file is part of PFM.
 *
 * PFM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PFM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PFM. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <iostream>
#include <string>
#include <stdint.h>

#include <sys/ioctl.h>


#define TERMINAL_MIN_WIDTH                    80U
#define TERMINAL_MIN_HEIGHT                   17U

#define LIST_VIEW_THRESHOLD_WIDTH            110UL

class Terminal {
    public:
        static unsigned long getWidth() {
            struct winsize w;
            ioctl(0, TIOCGWINSZ, &w);
            return w.ws_col;
        }

        static unsigned long getHeight() {
            struct winsize w;
            ioctl(0, TIOCGWINSZ, &w);
            return w.ws_row;
        }

        static inline unsigned long fullWidthMinimum() {
            return TERMINAL_MIN_WIDTH;
        }

        static inline unsigned long fullHeightMinimum() {
            return TERMINAL_MIN_HEIGHT;
        }

        static bool isOverWidthThreshold() {
            return (getWidth() > LIST_VIEW_THRESHOLD_WIDTH);
        } 
};
