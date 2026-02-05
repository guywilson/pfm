#pragma once

class CLIWidget {
    public:
        CLIWidget() {}
        virtual ~CLIWidget() {};

        virtual void show() = 0;
};
