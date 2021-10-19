#ifndef ___INTERRUPTABLE_CONTROL_H___
#define ___INTERRUPTABLE_CONTROL_H___

#include "pico/stdlib.h"
#include "hardware/gpio.h"

class Interruptable_Control{
    public:
        Interruptable_Control();
        virtual void handleInterruptEvent(uint gpio, uint32_t events);
};
#endif