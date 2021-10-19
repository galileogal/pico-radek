#include "cd4051controller.hpp"

Single_4051_chip::Single_4051_chip(Multi_4051_Controller* controller, uint gpio, bool initializePin)
{
    this->_controller = controller;
    this->_gpio = gpio;

    this->_currentValues = 0; 
    this->_lastValues    = 0;   //used for software debounce   

    if (initializePin) this->_initializePin();
    this->_initialize4051AllPinsConfig();
}

void Single_4051_chip::configure4051PinConfig(char pinIndex, bool enableDebounce, uint32_t debounceDelay_us)
{
    if (pinIndex >= MUX_CHIP_PINS_COUNT) return;

    this->_pinConfigs[pinIndex].bEnableDebounce  = enableDebounce;
    this->_pinConfigs[pinIndex].debounceDelay_us = debounceDelay_us;
    this->_pinConfigs[pinIndex].lastDebounceTime = nil_time;
}

void Single_4051_chip::_initializePin()
{
    gpio_init(this->_gpio);
    gpio_set_dir(this->_gpio, GPIO_IN);    
}

void Single_4051_chip::_initialize4051AllPinsConfig()
{
    for (char a = 0; a < MUX_CHIP_PINS_COUNT; a++){
        this->configure4051PinConfig(a, false);
    }
}

bool Single_4051_chip::_getBitValue(char src, char bitIndex)
{
    return (src >> bitIndex) & 1;
}

void Single_4051_chip::_setBitValue(bool value, char& dst, char bitIndex)
{
    if (value)
    {
        // set bit to 1
        dst |= 1 << bitIndex;
    }
    else
    {
        // set bit to 0
        dst &= ~(1 << bitIndex);
    }
}

void Single_4051_chip::updateValueFromPin(char currPin)
{
    bool valueChanged = false;

    // multi_4051_controller object sets the address to control which 4051 chip's pin outputs value
    // this value is then passed to microcontrollers input pin (at _gpio)
    // set a proper bit of currentValues according to this input pin's value
    bool currentReading = gpio_get(this->_gpio);

    // shoud we handle software debounce
    if (this->_pinConfigs[currPin].bEnableDebounce){
        // If the pin's state has changed (noise or not)
        if (currentReading != this->_getBitValue(this->_lastValues, currPin)){
            // reset debouncing timer
            this->_pinConfigs[currPin].lastDebounceTime = get_absolute_time();
        }

        if (absolute_time_diff_us(get_absolute_time(), this->_pinConfigs[currPin].lastDebounceTime) > this->_pinConfigs[currPin].debounceDelay_us)
        {
            // whatever the reading is at, it's been there for longer than the debounce
            // delay, so take it as the actual current state:

            if (currentReading != this->_getBitValue(this->_currentValues, currPin))
            {
                valueChanged = true;    // used by callbacks
                this->_setBitValue(currentReading, this->_currentValues, currPin);
            }
        }

        // save the reading. Next time through the loop, it'll be the lastValue:
        this->_setBitValue(currentReading, this->_lastValues, currPin);
    }
    else
    {
        if (currentReading != this->_getBitValue(this->_currentValues, currPin))
        {
            this->_setBitValue(currentReading, this->_currentValues, currPin);
            valueChanged = true;
        }        
    }

    if (valueChanged)
    {
        
    }
}

bool Single_4051_chip::getPinValue(char pinIndex)
{
    return this->_getBitValue(this->_currentValues, pinIndex);
}

void Single_4051_chip::setCallback(char pinIndex, pin_state_changed_callback cb)
{
    if (pinIndex >= MUX_CHIP_PINS_COUNT) return;
    this->_pinConfigs[pinIndex].callbacks.push_back(cb);
}

string Single_4051_chip::stringifyValues()
{
    string strout = "";
    
    for (int a = 0; a < 8; a++)
    {
        if ((this->_currentValues >> (7 - a)) & 1)
        {
            strout += "1";
        }
        else
        {
            strout += "0";
        }        
    }

    return strout;
}



Multi_4051_Controller::Multi_4051_Controller(uint addressPin_A, uint addressPin_B, uint addressPin_C, bool initializePins)
{
    // assign private variables
    this->_addressPin_A = addressPin_A;
    this->_addressPin_B = addressPin_B;
    this->_addressPin_C = addressPin_C;

    // initialize arrays containing information about single chips
    for (int a = 0; a < MULTI4051CONTROLLER_MAX_CHIPS; a++)
    {
        this->_chipsBank[a] = NULL;
    }

    // initialize GPIOs if requested - otherwise they need to be already initialized
    if (initializePins) this->_initializePins();

    gpio_put(this->_addressPin_A, false);
    gpio_put(this->_addressPin_B, false);
    gpio_put(this->_addressPin_C, false);

    this->_currentAddress = 0;
}

void Multi_4051_Controller::_initializePins()
{
    gpio_init(this->_addressPin_A);
    gpio_set_dir(this->_addressPin_A, GPIO_OUT);    

    gpio_init(this->_addressPin_B);
    gpio_set_dir(this->_addressPin_B, GPIO_OUT);    

    gpio_init(this->_addressPin_C);
    gpio_set_dir(this->_addressPin_C, GPIO_OUT);    
}

Single_4051_chip* Multi_4051_Controller::addDigitalChip(uint8_t chipIndex, uint gpio, bool initializePin)
{
    if (this->_chipsBank[chipIndex] != NULL) return NULL;
    
    Single_4051_chip* newChip = new Single_4051_chip(this, gpio, initializePin);
    this->_chipsBank[chipIndex] = newChip;

    return newChip;
}

void Multi_4051_Controller::updateAllDigitalValues()
{
    // iterate through all input pins of all managed 4051 chips
    for (char a = 0; a < 8; a++)
    {
        // set address pins
        gpio_put(this->_addressPin_A, (bool)(a & 0x01)); 
        gpio_put(this->_addressPin_B, (bool)(a & 0x02));
        gpio_put(this->_addressPin_C, (bool)(a & 0x04));
       
        this->_currentAddress = a;

        sleep_us(5);

        // iterate through all controlled chips
        for (int i = 0; i < MULTI4051CONTROLLER_MAX_CHIPS; i++)
        {
            if (this->_chipsBank[i] != NULL)
            {
                this->_chipsBank[i]->updateValueFromPin(a);    
            }
        }
    }
}

char Multi_4051_Controller::currentAddress()
{
    return this->_currentAddress;
}