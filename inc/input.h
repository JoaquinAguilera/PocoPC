#pragma once

namespace poco::input {

    enum input
    {
        INPUT_LEFT = 0,
        INPUT_RIGHT,
        INPUT_UP,
        INPUT_DOWN,
        INPUT_CONFIRM,
        INPUT_CANCEL,
        INPUT_RESET
    };

    void init();
    void shutdown();

    bool button_state(input Input);
    bool button_pressed(input Input);
}