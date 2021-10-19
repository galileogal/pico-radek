#include "./interruptable_control.hpp"

Interruptable_Control::Interruptable_Control(){

}

void Interruptable_Control::handleInterruptEvent(uint gpio, uint32_t events){
    // handle this event in derived classes   
}