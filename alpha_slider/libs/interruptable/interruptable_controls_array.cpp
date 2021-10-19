#include "./interruptable_controls_array.hpp"

// initialize static members
int InterruptableControlsArray::_activeControlsCount = 0;
Interruptable_Control* InterruptableControlsArray::controlsArray[] = {};


void gpio_callback(uint gpio, uint32_t events){      
    for (int a = 0; a < MAX_CONTROL_ITEMS; a++){    
        if (InterruptableControlsArray::controlsArray[a])
            (InterruptableControlsArray::controlsArray[a])->handleInterruptEvent(gpio, events);
    }
}



InterruptableControlsArray::InterruptableControlsArray(){

}

void InterruptableControlsArray::registerControl(Interruptable_Control* control){
    // initialize controlsArray if this method is called for the first time
    if (!InterruptableControlsArray::_activeControlsCount){
        for (int a = 0; a < MAX_CONTROL_ITEMS; a++){    
            InterruptableControlsArray::controlsArray[a] = NULL;
        }
    }

    // index at which we will store reference to control instance
    int firstAvailableIndex = -1;

    // first make sure this control isn't already registered
    for (int a = 0; a < MAX_CONTROL_ITEMS; a++){
        if (InterruptableControlsArray::controlsArray[a] == control){
            return;
        }
        else if (InterruptableControlsArray::controlsArray[a] == NULL && firstAvailableIndex == -1){
            firstAvailableIndex = a;
        }
    }

    if (firstAvailableIndex != -1){
        InterruptableControlsArray::controlsArray[firstAvailableIndex] = control;
        InterruptableControlsArray::_activeControlsCount++;
    }
}

void InterruptableControlsArray::unregisterControl(Interruptable_Control* control){
    // remove the reference to this control from array
    for (int a = 0; a < MAX_CONTROL_ITEMS; a++){
        if (InterruptableControlsArray::controlsArray[a] == control){
            InterruptableControlsArray::controlsArray[a] = NULL;
            InterruptableControlsArray::_activeControlsCount--;
            return;
        }
    }
}

void InterruptableControlsArray::setIRQHandler(uint gpio, uint32_t events, bool enable){
    gpio_set_irq_enabled_with_callback(gpio, events, enable, &gpio_callback);
}