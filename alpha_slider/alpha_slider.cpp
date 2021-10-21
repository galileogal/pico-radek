/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "hardware/spi.h"
#include "hardware/adc.h"

#include "libs/lcd_i2c.hpp"
#include "libs/mcp425X.hpp"
#include "libs/rotary_encoder.hpp"
#include "libs/button_wcallback.hpp"
#include "libs/blinking_led.hpp"
#include "libs/slider_motor/slider_motor.hpp"

/*
    I2C bus settings
*/
#define I2C_PORT i2c0
#define I2C_0_SDA 4
#define I2C_0_SCL 5


/*
    SPI bus settings

    GPIO 16 (pin 21) MISO/spi0_rx-> SDO/SDO 
    GPIO 17 (pin 22) Chip select -> CSB/!CS 
    GPIO 18 (pin 24) SCK/spi0_sclk -> SCL/SCK
    GPIO 19 (pin 25) MOSI/spi0_tx -> SDA/SDI
*/

#define PIN_MISO 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19

#define SPI_PORT spi0


/* 
    Rotary encoder 1 pin settings
*/
#define PIN_ROTARY1_A 10
#define PIN_ROTARY1_B 11
#define PIN_ROTARY_BUTTON 2

/*
    GPIO pin settings
*/
#define LED_PIN 25
#define SLIDER_CONTROL_BUTTON_PIN 6
#define SLIDER_SENSOR_PIN 12
#define LED_BUTTON_PIN 3
#define ADC_PIN 28


enum enmDisplayMenuPosition {
    welcome = 1,
    center_head_on_slider = 2,
    normal_operation = 3,
    set_speed = 4,
    set_turnback_duration = 5,
    set_turnback_speed_curve = 6,
    set_backbround_light = 7,
    exit = 8
};


/*
    Global variables
*/

const float conversion_factor = 3.3f / (1 << 12);   // 12-bit conversion, assume max value == ADC_VREF == 3.3 V
uint16_t result;
float rotaryValue;
uint8_t rotaryValuePositive;
int8_t speedValue = -99;
uint8_t backlightIntesity = 255, newBacklightIntesity = 255;
bool menuValueEditorEnabled = false;

strctControllerSetting_t currentMotorSetting;
enmDisplayMenuPosition displayMenuPos = welcome, menuPosBeforEnteringMenu = normal_operation;

MCP425X* potentiometer1 = NULL;
Blinking_LED* buttonLED = NULL;
Slider_motor* motorController = NULL;
Rotary_encoder* rotEncoder1 = NULL;
Button_With_Callback* rotButton = NULL;
Button_With_Callback* sliderControlButton = NULL;
Display* ptrLCD = NULL;

bool motorControlActive = false;

uint64_t lastRotaryEncoderActionTimestamp = 0;
uint32_t maxRotaryEncoderIdleTime = 5 * 1000000;    // idle time after which menu will be deactivated

uint slilderMotorMinSettingIndex = 0;
uint slilderMotorMaxSettingIndex = 0;

float minTurnbackTime = 0.6;
float maxTurnbackTime = 6;
float turnbackStep = 0.2;
float currentTurnbackDuration = 1, newTurnbackDuration = 1;


/*
    Handlers
*/
void _welcomeMessage(Display* lcd){
    const char* tekst =  "ALPHA SLIDER 0.1";
    const char* tekst1 = " Radek Brzozka";

    lcd->home();
    lcd->write(tekst);
    lcd->move(0, 1);
    lcd->write(tekst1);

    sleep_ms(2000);

    lcd->clear();

    displayMenuPos = normal_operation;
}




void sliderButtonPressed(uint button_PIN){
    if (!motorControlActive && displayMenuPos != center_head_on_slider){
        // prompt user to center camera head on slider
        displayMenuPos = center_head_on_slider;
        buttonLED->blinkOn(400, 50);
    }
    else if (!motorControlActive && displayMenuPos == center_head_on_slider){
        // this is the second press of the button - user has confirmed the camera head is ready
        motorControlActive = true;
        
        motorController->setCurrentSettingIndex(rotaryValue);
        motorController->activate();

        buttonLED->setState(true);

        displayMenuPos = normal_operation;
    }
    else if (motorControlActive){
        // stop the motor and deactivate control
        motorControlActive = false;
        motorController->stop();
        motorController->deactivate();
        buttonLED->setState(false);
    }    
}

void sliderButtonHeld(uint button_PIN){
    buttonLED->blinkOn(400, rotaryValuePositive);
}

void updateResistorValueCallback(uint resitorValue){
    potentiometer1->sendCommand(MCP425X::wiper0writeAddr, resitorValue);
    potentiometer1->sendCommand(MCP425X::wiper1writeAddr, resitorValue);
}

void onStartTurnback(void){
    buttonLED->blinkOn(200, 50);
}

void onEndTurnback(void){
    buttonLED->setState(true);
}

void updateMotorSetting(strctControllerSetting_t newSetting){
    currentMotorSetting = newSetting;
}

void onRotaryEncoderValueChange(float newValue){
    if (rotaryValue != newValue){           
        rotaryValue = newValue;    
        motorController->setCurrentSettingIndex(rotaryValue);
        currentMotorSetting = motorController->getCurrentSetting();
    }    
}

void onRotaryEncoderValueChangeInMenu(float newValue){
    lastRotaryEncoderActionTimestamp = time_us_64();

    int val = newValue;

    switch (val){
        case 1:
            displayMenuPos = set_turnback_duration;
            break;

        case 2:
            displayMenuPos = set_backbround_light;
            break;        

        case 3:
            displayMenuPos = exit;
            break;
    }
}

void onRotaryEncoderValueChangeTurnback(float newValue){
    lastRotaryEncoderActionTimestamp = time_us_64();

    newTurnbackDuration = newValue;
}

void onRotaryEncoderValueChangeBackground(float newValue){
    lastRotaryEncoderActionTimestamp = time_us_64();

    newBacklightIntesity = newValue;    
}

void setRotaryEncoderToMotorSpeedSettings(){
    rotEncoder1->setBoundaries(slilderMotorMinSettingIndex, slilderMotorMaxSettingIndex, 1, stop);    
    rotEncoder1->setOnValueChangeCallback(onRotaryEncoderValueChange);
    rotEncoder1->setCurrentValue(motorController->getCurrentSettingIndex());
}


void resetMenuToNormalOperation(){
    displayMenuPos = set_speed;
    menuValueEditorEnabled = false;
    setRotaryEncoderToMotorSpeedSettings();
}

void buttonPressed(uint button_PIN){    
    lastRotaryEncoderActionTimestamp = time_us_64();

    switch (displayMenuPos){
        case welcome:
            break;  // do nothing

        case center_head_on_slider:
            break;

        case normal_operation:
        case set_speed:
            if (!motorControlActive){
                menuPosBeforEnteringMenu = normal_operation;    // this is where we'll go back after exiting menu
                displayMenuPos = set_turnback_duration;

                //configure rotary encoder
                rotEncoder1->setBoundaries(1, 3, 1, stop);
                rotEncoder1->setOnValueChangeCallback(onRotaryEncoderValueChangeInMenu);
                rotEncoder1->setCurrentValue(1);
            }
            break;

        case set_turnback_duration:
            if (!menuValueEditorEnabled){
                menuValueEditorEnabled = true;

                rotEncoder1->setBoundaries(minTurnbackTime, maxTurnbackTime, turnbackStep, stop);                
                rotEncoder1->setOnValueChangeCallback(onRotaryEncoderValueChangeTurnback);
                rotEncoder1->setCurrentValue(currentTurnbackDuration);
            }
            else {
                menuValueEditorEnabled = false;

                //configure rotary encoder
                rotEncoder1->setBoundaries(1, 3, 1, stop);
                rotEncoder1->setOnValueChangeCallback(onRotaryEncoderValueChangeInMenu);
                rotEncoder1->setCurrentValue(1);
            }
            break;

        case set_backbround_light:
            if (!menuValueEditorEnabled){
                menuValueEditorEnabled = true;

                rotEncoder1->setBoundaries(0, 255, 5, stop);
                rotEncoder1->setOnValueChangeCallback(onRotaryEncoderValueChangeBackground);
                rotEncoder1->setCurrentValue(backlightIntesity);
            }
            else {
                menuValueEditorEnabled = false;

                //configure rotary encoder
                rotEncoder1->setBoundaries(1, 3, 1, stop);
                rotEncoder1->setOnValueChangeCallback(onRotaryEncoderValueChangeInMenu);
                rotEncoder1->setCurrentValue(2);
            }
            break;

        case exit:
            resetMenuToNormalOperation();
            break;

    }
}

void buttonDown(uint button_PIN){
    
}

void buttonUp(uint button_PIN){
    
}



int main() {    
    stdio_init_all();


    // This example will use I2C0 on GPIO4 (SDA) and GPIO5 (SCL)
    i2c_init(I2C_PORT, 200 * 1000);
    gpio_set_function(I2C_0_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_0_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(6);
    gpio_pull_up(7);

        
    // This example will use SPI0 at 0.5MHz.
    spi_init(SPI_PORT, 500 * 1000);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    

    // initialize digital potentiometer
    potentiometer1 = new MCP425X(SPI_PORT, PIN_CS, true);

       
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    sliderControlButton = new Button_With_Callback(SLIDER_CONTROL_BUTTON_PIN, hi, true);
    sliderControlButton->setButtonOnPressedCallback(sliderButtonPressed);
    sliderControlButton->setButtonHeldDownCallback(sliderButtonHeld, 500000);

    buttonLED = new Blinking_LED(LED_BUTTON_PIN, true);
    buttonLED->setState(false);
        

    /*
        Make sure SPI pins and potentiometer are initialized at this point
    */
    motorController = new Slider_motor(updateResistorValueCallback, SLIDER_SENSOR_PIN, true);
    motorController->getSettingsIndexLimits(slilderMotorMinSettingIndex, slilderMotorMaxSettingIndex);    
    motorController->setTurnBackOptions(1500000, linear);
    currentMotorSetting = motorController->getCurrentSetting();
    motorController->setOnStartTurnbackCallback(onStartTurnback);
    motorController->setOnEndTurnbackCallback(onEndTurnback);
    motorController->setOnUpdateControllerSettings(updateMotorSetting);

    // initialize rotary button
    rotButton = new Button_With_Callback(PIN_ROTARY_BUTTON, lo, true);    
    rotButton->setButtonOnPressedCallback(buttonPressed);
    rotButton->setButtonOnDownCallback(buttonDown);
    rotButton->setButtonOnUpCallback(buttonUp);
    
    

    

    
    // Initiate ADC pin to control voltage
    adc_init();
    adc_gpio_init(ADC_PIN);
    adc_select_input(2);
    
    // initialize main selector - rotary encoder    
    rotEncoder1 = new Rotary_encoder(PIN_ROTARY1_A, PIN_ROTARY1_B, false);
    setRotaryEncoderToMotorSpeedSettings();
    
    
    // initialize LCD display
    Display lcd = Display(I2C_PORT);
    ptrLCD = &lcd;
    lcd.color(backlightIntesity, backlightIntesity, backlightIntesity);  
    lcd.cursor(false);

    

    
    char buffer1[100], buffer2[100];
    int8_t wiperValue = 0;            
    bool lastButtonPressed = true;
    bool buttonPressed = true;


    while (1) {        
        result = adc_read();

        // update devices' state
        rotButton->update();
        sliderControlButton->update();
        buttonLED->update();
        motorController->update();
        rotEncoder1->update();
                

        switch (displayMenuPos){
            case welcome:
                _welcomeMessage(&lcd);
                lcd.clear();
                break;

            case center_head_on_slider:
                sprintf(buffer1, "  CENTER DOLLY  ");
                sprintf(buffer2, "  BTWN MARKERS  ");
                break;

            case normal_operation:
            case set_speed:
                if (currentMotorSetting.direction != 'S'){
                    sprintf(buffer1, "Spd: %3d  Dir: %c", currentMotorSetting.speedValue, currentMotorSetting.direction);
                }
                else {
                    sprintf(buffer1, "  MOTOR STOPPED ");
                }

                sprintf(buffer2, "Trnbk dur: %1.1fs ", currentTurnbackDuration);
                break;
            
            case set_turnback_duration:
                if (menuValueEditorEnabled){
                    if (newTurnbackDuration != currentTurnbackDuration){
                        currentTurnbackDuration = newTurnbackDuration;
                        motorController->setTurnBackOptions(currentTurnbackDuration * 1000000, linear);
                    }

                    sprintf(buffer1, "   [SET VALUE]  ");
                    sprintf(buffer2, "Duration: %1.1f s ", currentTurnbackDuration);
                }
                else{
                    sprintf(buffer1, "     [MENU]     ");
                    sprintf(buffer2, "1. TURNBACK DUR.");
                }
                
                if (time_us_64() - lastRotaryEncoderActionTimestamp > maxRotaryEncoderIdleTime){
                    resetMenuToNormalOperation();
                }
                break;

            case set_backbround_light:
                if (menuValueEditorEnabled){
                    if (newBacklightIntesity != backlightIntesity){                        
                        //avoid updating any screen data in irq handler
                        backlightIntesity = newBacklightIntesity;
                        lcd.color(backlightIntesity, backlightIntesity, backlightIntesity);
                        sleep_ms(5);
                    }

                    sprintf(buffer1, "   [SET VALUE]  ");
                    sprintf(buffer2, "Intensity: %3d  ", backlightIntesity);
                }
                else {
                    sprintf(buffer1, "     [MENU]     ");
                    sprintf(buffer2, "2. BACKGND LIGTH");
                }
                
                if (time_us_64() - lastRotaryEncoderActionTimestamp > maxRotaryEncoderIdleTime){
                    resetMenuToNormalOperation();
                }
                break;

            case exit:
                sprintf(buffer1, "     [MENU]     ");
                sprintf(buffer2, "3. EXIT         ");
                if (time_us_64() - lastRotaryEncoderActionTimestamp > maxRotaryEncoderIdleTime){
                    resetMenuToNormalOperation();
                }
                break;

            default:
                break;
            
        };        

        lcd.home();
        lcd.write(buffer1);
        lcd.move(0, 1);
        lcd.write(buffer2);

        
        //printf("%d\n", time_us_64());
        
        sleep_ms(1);
    }

    return 0;
}




