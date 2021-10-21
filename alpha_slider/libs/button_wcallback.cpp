#include "./button_wcallback.hpp"

Button_With_Callback::Button_With_Callback(uint button_PIN, enmPressedState pressedState, bool initializePinForDigitalInput){
    this->_button_PIN = button_PIN;    

    this->_lastButtonPressed = true;
    this->_buttonPressed = true;
    this->_state = false;
    this->_pressedState = pressedState;

    if (initializePinForDigitalInput){
        gpio_init(this->_button_PIN);
        gpio_set_dir(this->_button_PIN, GPIO_IN);  
    }
}

void Button_With_Callback::setButtonOnPressedCallback(buttonOnPressedCallback callback){
    this->_buttonOnPressedCallback = callback;
}

void Button_With_Callback::setButtonOnDownCallback(buttonOnDownCallback callback){
    this->_buttonOnDownCallback = callback;
}

void Button_With_Callback::setButtonOnUpCallback(buttonOnUpCallback callback){
    this->_buttonOnUpCallback = callback;
}

void Button_With_Callback::setButtonHeldDownCallback(buttonHeldDownCallback callback, uint64_t minimumTimeUS){
    this->_minimumTimeUSPressedForHoldEvent = minimumTimeUS;
    this->_buttonHeldDownCallback = callback;
}


void Button_With_Callback::update(){
    this->_buttonPressed = gpio_get(this->_button_PIN);

    bool lastState = this->_state;

    // has the state changed?
    if (this->_buttonPressed != this->_lastButtonPressed){                        
        if (this->_buttonPressed == this->_pressedState){
            this->_state = true;

            // also remember when the pushdown happened
            this->_lastButtonPresseduSTimestamp = time_us_64();
            this->_buttonHeldDownEventTriggered = this->_buttonHeldDownCallback == NULL;  // set to true only if handler isn't set, otherwise set to false
        }
        else {
            this->_state = false;
        }

        // should we trigger onDown event?
        if (!lastState && this->_state && this->_buttonOnDownCallback) this->_buttonOnDownCallback(this->_button_PIN);
        
        // should we trigger onPressed / onUpEvent
        if (lastState && !this->_state){
            if (!this->_buttonHeldFlag){
                // dont call default handlers if this was a long button hold event
                if (this->_buttonOnUpCallback) this->_buttonOnUpCallback(this->_button_PIN);
                if (this->_buttonOnPressedCallback) this->_buttonOnPressedCallback(this->_button_PIN);
            }
            else {
                this->_buttonHeldFlag = false;
            }            
        }
    }

    // if the key is pressed and onHeld event handler is set - check if it should be called
    if (this->_state && this->_buttonHeldDownCallback && !this->_buttonHeldDownEventTriggered && time_us_64() - this->_lastButtonPresseduSTimestamp >= this->_minimumTimeUSPressedForHoldEvent){
        this->_buttonHeldDownEventTriggered = true;
        this->_buttonHeldDownCallback(this->_button_PIN);
        this->_buttonHeldFlag = true;
    }

    this->_lastButtonPressed = this->_buttonPressed;
}