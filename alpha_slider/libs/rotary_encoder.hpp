#ifndef ___ROTARY_ENCODER_H___
#define ___ROTARY_ENCODER_H___

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "./interruptable/interruptable_control.hpp"
#include "./interruptable/interruptable_controls_array.hpp"

#define MAX_ROTARY_ENCODERS 100

typedef void (*onValueChangeCallbackType)(float newValue);

enum enmEncoderBoundaryReachedAction {
    stop = 0,
    go_around = 1
};

class Rotary_encoder : Interruptable_Control {
    private:
        uint _encoderPhaseA_PIN, _encoderPhaseB_PIN;

        uint _lastMSB, _lastLSB, _lastEncoded;

        float _minVal, _maxVal, _currVal, _lastValueEmitted, _step = 1;
        enmEncoderBoundaryReachedAction _outOfRangeAction = stop;
      
        onValueChangeCallbackType _onValueChangeCallback = NULL;

    public:       
        Rotary_encoder(uint encoderPhaseA_PIN, uint encoderPhaseB_PIN, bool initializeEncoderPinsForDigitalInput = false);
        ~Rotary_encoder();


        void setBoundaries(float minVal, float maxVal, float step, enmEncoderBoundaryReachedAction outOfRangeAction = stop);
        void setCurrentValue(float newValue);
        void setOnValueChangeCallback(onValueChangeCallbackType cb);
        
        float readValue();

        

        // derived from Interruptable_Control
        void handleInterruptEvent(uint gpio, uint32_t events);
        
};



#endif
