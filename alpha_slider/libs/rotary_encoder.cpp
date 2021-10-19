#include "./rotary_encoder.hpp"
#include <math.h>

Rotary_encoder::Rotary_encoder(uint encoderPhaseA_PIN, uint encoderPhaseB_PIN, bool initializeEncoderPinsForDigitalInput) : Interruptable_Control(){
    if (InterruptableControlsArray::_activeControlsCount >= MAX_CONTROL_ITEMS){
        printf("Active interrupt devices count exceeded maximum number!");
        return;
    };
    
    //this->registerInActiveEncodersArray();
    InterruptableControlsArray::registerControl(this);
    
    this->_encoderPhaseA_PIN = encoderPhaseA_PIN;
    this->_encoderPhaseB_PIN = encoderPhaseB_PIN;

    // initialize both pins if necessary, remember to pull them up!
    if (initializeEncoderPinsForDigitalInput){
        gpio_init(this->_encoderPhaseA_PIN);
        gpio_set_dir(this->_encoderPhaseA_PIN, GPIO_IN);        

        gpio_init(this->_encoderPhaseB_PIN);
        gpio_set_dir(this->_encoderPhaseB_PIN, GPIO_IN);        
    }

    // initialize readout values
    this->_lastMSB = 0;
    this->_lastLSB = 0;
    this->_lastEncoded = 0;
    this->_currVal = 0;
    this->_lastValueEmitted = 0;

    InterruptableControlsArray::setIRQHandler(this->_encoderPhaseA_PIN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);
    InterruptableControlsArray::setIRQHandler(this->_encoderPhaseB_PIN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);
}

Rotary_encoder::~Rotary_encoder(){
    // deactivate IRQ handling
    InterruptableControlsArray::setIRQHandler(this->_encoderPhaseA_PIN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, false);
    InterruptableControlsArray::setIRQHandler(this->_encoderPhaseB_PIN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, false);
    
    // unregister from controls array
    InterruptableControlsArray::unregisterControl(this);
}

void Rotary_encoder::setBoundaries(float minVal, float maxVal, float step, enmEncoderBoundaryReachedAction outOfRangeAction){
    if (!step) return;      // otherwise we'll have division by zero!

    this->_step   = step;

    this->_minVal = (minVal / step) * 4;
    this->_maxVal = (maxVal / step) * 4;
    this->_outOfRangeAction = outOfRangeAction;
}

void Rotary_encoder::setCurrentValue(float newValue){
    float calculatedNewValue = (newValue / this->_step) * 4;
    
    if (abs(newValue) / this->_step == floor(abs(newValue) / this->_step) &&    // make sure new value is multiple of step!
        calculatedNewValue >= this->_minVal && 
        calculatedNewValue <= this->_maxVal){
            
            this->_currVal = calculatedNewValue;
        }
        

    if (this->_onValueChangeCallback){
        this->_lastValueEmitted = this->readValue();
        this->_onValueChangeCallback(this->readValue());
    }
}

void Rotary_encoder::setOnValueChangeCallback(onValueChangeCallbackType cb){
    this->_onValueChangeCallback = cb;
}

float Rotary_encoder::readValue(){
    return floor(this->_currVal / 4) * this->_step;
}



void Rotary_encoder::handleInterruptEvent(uint gpio, uint32_t events){
    // only service this interrupt if given pin belongs to this encoder
    if (gpio == this->_encoderPhaseA_PIN || gpio == this->_encoderPhaseB_PIN){
        int MSB = gpio_get(this->_encoderPhaseA_PIN); //MSB = most significant bit
        int LSB = gpio_get(this->_encoderPhaseB_PIN); //LSB = least significant bit

        int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number
        int sum  = (this->_lastEncoded << 2) | encoded; //adding it to the previous encoded value

        if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {
            this->_currVal++;

            if (this->_currVal > this->_maxVal){
                if (this->_outOfRangeAction == stop){
                    this->_currVal = this->_maxVal;
                }
                else if (this->_outOfRangeAction == go_around){
                    this->_currVal = this->_minVal;
                }
            }
        }

        if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000){
            this->_currVal--;

            if (this->_currVal < this->_minVal){
                if (this->_outOfRangeAction == stop){
                    this->_currVal = this->_minVal;
                }
                else if (this->_outOfRangeAction == go_around){
                    this->_currVal = this->_maxVal;
                }
            }
        }

        this->_lastEncoded = encoded; //store this value for next time

        // has value changed?
        if (this->_onValueChangeCallback && this->_lastValueEmitted != floor(this->_currVal / 4) * this->_step){
            this->_lastValueEmitted = floor(this->_currVal / 4) * this->_step;
            this->_onValueChangeCallback(this->readValue());
        }
    }
}