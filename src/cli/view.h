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
