#ifndef ___INTERRUPTABLE_CONTROLS_ARRAY___
#define ___INTERRUPTABLE_CONTROLS_ARRAY___

#include "./interruptable_control.hpp"

#define MAX_CONTROL_ITEMS 100

class InterruptableControlsArray{
    public:
        static Interruptable_Control* controlsArray[MAX_CONTROL_ITEMS];
        static int _activeControlsCount;
                
        InterruptableControlsArray();

        static void registerControl(Interruptable_Control* control);
        static void unregisterControl(Interruptable_Control* control);
        static void setIRQHandler(uint gpio, uint32_t events, bool enable);
};

#endif