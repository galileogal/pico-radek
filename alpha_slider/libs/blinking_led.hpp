#ifndef ___BLINKING_LED___
#define ___BLINKING_LED___

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

class Blinking_LED {
    private:
        uint _led_PIN;
        bool _state = false;
        uint32_t _periodStartTimestamp;    
        uint16_t _blinkPeriodMS;        // blink periond in milliseconds
        uint _blinkDutyCyclePercent;    // blink duty cycle in percent (0 - 100) of blink period
        bool _blinking = false;         // is blinking mode enabled (otherwise LED behaves normally - on/off only)


    public:
        Blinking_LED(uint led_PIN, bool initializeLEDPinForDigitalOutput = false);

        void blinkOn(uint16_t blinkPeriodMS, uint blinkDutyCyclePercent = 50);
        void blinkOff();
        void setState(bool newState);   // calling this method will disable blinking!

        void update();

};

#endif