#ifndef ___CD4051CONTROLLER_H___
#define ___CD4051CONTROLLER_H___

#include <string>
#include <vector>
#include "pico/stdlib.h"

#define MULTI4051CONTROLLER_MAX_CHIPS 5
#define MUX_CHIP_PINS_COUNT 8

using namespace std;

class Multi_4051_Controller;

typedef void (* pin_state_changed_callback)(char, char, bool);

typedef struct PinConfig_4051 {
    bool bEnableDebounce;
    uint32_t debounceDelay_us;
    absolute_time_t lastDebounceTime; 
    vector<pin_state_changed_callback> callbacks;
} PinConfig_4051;




class Single_4051_chip
{
    private:
        char _currentValues;    // this register stores all 8 values from chip's pins in single bits
        char _lastValues;       // this register stores all 8 values from chip's pins in single bits 
                                // read in the previous iteration (used for software debounce)
        PinConfig_4051 _pinConfigs[MUX_CHIP_PINS_COUNT];  // array containig setup information for all i/o pins of 4051 chip
        uint _gpio;             // microcontroller's gpio used EXCLUSIVELY to read values from this chip
        Multi_4051_Controller* _controller; // parent controller


        void _initializePin();
        void _initialize4051AllPinsConfig();

        bool _getBitValue(char src, char bitIndex);
        void _setBitValue(bool value, char& dst, char bitIndex);
        
    public:
        Single_4051_chip(Multi_4051_Controller* controller, uint gpio, bool initializePin = false);
        
        void configure4051PinConfig(char pinIndex, bool enableDebounce = false, uint32_t debounceDelay_us = 200 * 1000);
        void updateValueFromPin(char currPin);
        bool getPinValue(char pinIndex);
        void setCallback(char pinIndex, pin_state_changed_callback cb);
        string stringifyValues();

};

class Multi_4051_Controller
{
    private:
        uint _addressPin_A;     // gpio connected to A address pin of ALL 4051 chips
        uint _addressPin_B;     // gpio connected to B address pin of ALL 4051 chips
        uint _addressPin_C;     // gpio connected to C address pin of ALL 4051 chips

        char _currentAddress;   // stores the address set via all three address pins

        Single_4051_chip* _chipsBank[MULTI4051CONTROLLER_MAX_CHIPS];

        void _initializePins();

    public:
        Multi_4051_Controller(uint addressPin_A, uint addressPin_B, uint addressPin_C, bool initializePins = false);
        
        Single_4051_chip* addDigitalChip(uint8_t chipIndex, uint gpio, bool initializePin = false);
        void updateAllDigitalValues();
        char currentAddress();  // returns the value of currently set address (address is set via address pins)


};



#endif