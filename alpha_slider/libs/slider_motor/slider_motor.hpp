#ifndef ___SLIDER_MOTOR_H___
#define ___SLIDER_MOTOR_H___

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include "../interruptable/interruptable_control.hpp"
#include "../interruptable/interruptable_controls_array.hpp"

#define ILLEGAL_MOTOR_SETTINGS_ARRAY_INDEX 999

enum enmSlider_Motor_Turnback_Speed_Curve {
    linear = 1,
    sin = 2
};

typedef struct strctControllerSetting {
    uint8_t resistorValue;
    uint8_t speedValue;
    char    direction;
} strctControllerSetting_t;



typedef void (*setResistorValueCallback)(uint resitorValue);  // used for controlling external resistor
typedef void (*genericSliderMotorCallback)(void);
typedef void (*updateControllerSettingsCallback)(strctControllerSetting_t);





class Slider_motor : Interruptable_Control {
    private:
        u_int8_t _sensor_PIN;

        u_int _settings_array_size;
        u_int _current_setting_index;      

        bool  _control_activated = false;                
        
        bool  _turnback_in_progress = false;
        uint64_t _turnBackTime_us = 500000;
        enmSlider_Motor_Turnback_Speed_Curve _speedCurve = sin;
        u_int _turnback_start_setting_index = 0;
        u_int _turnback_final_setting_index = 0;
        u_int _turnback_current_setting_index = ILLEGAL_MOTOR_SETTINGS_ARRAY_INDEX;
        uint64_t _turnback_time_started = 0;


        u_int8_t _lastEncoded = 0; // used for calculating signals from IR sensor

        setResistorValueCallback    _setResistorCallback = NULL;
        genericSliderMotorCallback  _onStartTurnback = NULL;
        genericSliderMotorCallback  _onEndTurnback = NULL;
        updateControllerSettingsCallback _onUpdateControllerSettings = NULL;


        u_int getSettingIndex(uint8_t speed, char direction);
        void  updateResistorValue();

    public:
        Slider_motor(setResistorValueCallback setResistorCallback, u_int8_t sensor_PIN, bool initializeSensorPinForDigitalInput = false);

        void activate();        // start sending commands to the motor
        void deactivate();      // stop sending commands to the motor


        void getSettingsIndexLimits(uint& min, uint& max);
        strctControllerSetting_t getCurrentSetting();        
        u_int getCurrentSettingIndex();
        void setCurrentSettingIndex(u_int settingIndex);
        void stop();
        void setTurnBackOptions(uint64_t turnBackTime_us, enmSlider_Motor_Turnback_Speed_Curve speedCurve = sin);
        void turnBack();
        void setOnStartTurnbackCallback(genericSliderMotorCallback cb);
        void setOnEndTurnbackCallback(genericSliderMotorCallback cb);
        void setOnUpdateControllerSettings(updateControllerSettingsCallback cb);
        
        void update();

        // derived from Interruptable_Control
        void handleInterruptEvent(uint gpio, uint32_t events);
};

#endif