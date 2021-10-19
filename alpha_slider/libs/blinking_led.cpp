#include "./blinking_led.hpp"

Blinking_LED::Blinking_LED(uint led_PIN, bool initializeLEDPinForDigitalOutput){
    this->_led_PIN = led_PIN;

    if (initializeLEDPinForDigitalOutput){
        gpio_init(this->_led_PIN);
        gpio_set_dir(this->_led_PIN, GPIO_OUT);
        gpio_put(this->_led_PIN, this->_state);
    }
}

void Blinking_LED::blinkOn(uint16_t blinkPeriodMS, uint blinkDutyCyclePercent){
    /*
    uint _led_PIN;
    bool _state = false;
    uint32_t _periodStartTimestamp;      
    uint32_t _lastStateChangeTimestamp;
    uint16_t _blinkPeriodMS;        // blink periond in milliseconds
    uint _blinkDutyCyclePercent;    // blink duty cycle in percent (0 - 100) of blink period
    bool _blinking = false;         // is blinking mode enabled (otherwise LED behaves normally - on/off only)
    */
   
   this->_blinkPeriodMS = blinkPeriodMS;
   this->_blinkDutyCyclePercent = (blinkDutyCyclePercent >= 0 && blinkDutyCyclePercent <= 100) ? blinkDutyCyclePercent : 50;

   this->_periodStartTimestamp = time_us_32();
       
   this->_state = true;
   gpio_put(this->_led_PIN, this->_state);

   this->_blinking = true;
}


void Blinking_LED::blinkOff(){
    this->_blinking = false;
    gpio_put(this->_led_PIN, this->_state);
}

void Blinking_LED::setState(bool newState){
    this->_blinking = false;

    this->_state = newState;
    gpio_put(this->_led_PIN, this->_state);
}   

void Blinking_LED::update(){    
    if (!this->_blinking){
        // this is easy
        gpio_put(this->_led_PIN, this->_state);
        return;
    }

    // if were in blinking mode, we must chceck which part of the period are we in
    uint32_t now = time_us_32();
    uint32_t periodAdvanceTime = now - this->_periodStartTimestamp;

    if (periodAdvanceTime < 0){
        // time_us_32() wraps every 1 hour 11 minutes, so we must be ready for situation when value of 'now' is lower then '_periodStartTimestamp'
        this->_periodStartTimestamp = now;
        return;
    }

    // is the period over
    if (periodAdvanceTime >= this->_blinkPeriodMS * 1000){          
        this->_periodStartTimestamp = now;
        this->_state = true;
        gpio_put(this->_led_PIN, this->_state);
        return;
    }

    // we're within the period span, but where
    uint32_t progressPercent = ((periodAdvanceTime) / (this->_blinkPeriodMS * 10)); // multiplying by 10 is shorthand for (* 100) / (* 1000)
    

    if (progressPercent <= this->_blinkDutyCyclePercent){
        this->_state = true;
    }
    else this->_state = false;

    // finally set the value
    gpio_put(this->_led_PIN, this->_state);
}