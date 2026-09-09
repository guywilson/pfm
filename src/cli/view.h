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

#include "widget.h"

class CLIView : public CLIWidget {
    private:
        std::string title;

    protected:
        void printTitle() {
            std::cout << "*** " << title << " ***" << std::endl << std::endl;
        }

    public:
        CLIView() : CLIWidget() {}

        CLIView(const std::string & title) : CLIWidget() {
            setTitle(title);
        }

        void setTitle(const std::string & title) {
            this->title = title;
        }

        void show(const std::string & title) {
            printTitle();
        }

        void show() override {
            show(this->title);
        }
};
