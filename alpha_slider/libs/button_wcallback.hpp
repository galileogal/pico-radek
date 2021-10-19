#ifndef ___BUTTON_WCALLBACK___
#define ___BUTTON_WCALLBACK___

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

enum enmPressedState {
    lo = 0,
    hi = 1
};

typedef void (*buttonOnPressedCallback)(uint button_PIN);  // triggered when button is first pressed and then released
typedef void (*buttonOnDownCallback)(uint button_PIN);     // triggered when button is pushed down (but not released yet!)
typedef void (*buttonOnUpCallback)(uint button_PIN);       // triggered when button is released (after being pushed first)
typedef void (*buttonHeldDownCallback)(uint button_PIN);                          // triggered when button stays pushed for a preset minimal amount of time


class Button_With_Callback {
    private:
        uint _button_PIN;
        bool _lastButtonPressed = true;
        bool _buttonPressed = true;
        bool _state = false;
        bool _buttonHeldFlag = false;
        enmPressedState _pressedState = lo;

        uint64_t _lastButtonPresseduSTimestamp = 0;
        uint64_t _minimumTimeUSPressedForHoldEvent = 0;
        bool _buttonHeldDownEventTriggered = false;

        buttonOnPressedCallback _buttonOnPressedCallback = NULL;
        buttonOnDownCallback _buttonOnDownCallback = NULL;
        buttonOnUpCallback _buttonOnUpCallback = NULL;
        buttonHeldDownCallback _buttonHeldDownCallback = NULL;

    public:
        Button_With_Callback(uint button_PIN, enmPressedState pressedState = lo, bool initializePinForDigitalInput = false);

        void setButtonOnPressedCallback(buttonOnPressedCallback callback);
        void setButtonOnDownCallback(buttonOnDownCallback callback);
        void setButtonOnUpCallback(buttonOnUpCallback callback);
        void setButtonHeldDownCallback(buttonHeldDownCallback callback, uint64_t minimumTimeUS);

        void update();  // read pin state and update state if necessary

};

#endif