#include "./slider_motor.hpp"

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 



strctControllerSetting_t AVAILABLE_CONTROLLER_SETTINGS_TABLE[195] = {
    {6,97,'L'},
    {7,96,'L'},
    {8,95,'L'},
    {9,94,'L'},
    {10,93,'L'},
    {11,92,'L'},
    {12,91,'L'},
    {14,90,'L'},
    {15,89,'L'},
    {16,88,'L'},
    {17,87,'L'},
    {18,86,'L'},
    {19,85,'L'},
    {20,84,'L'},
    {22,83,'L'},
    {23,82,'L'},
    {24,81,'L'},
    {25,80,'L'},
    {26,79,'L'},
    {27,78,'L'},
    {28,77,'L'},
    {29,76,'L'},
    {31,75,'L'},
    {32,74,'L'},
    {33,73,'L'},
    {34,72,'L'},
    {35,71,'L'},
    {36,70,'L'},
    {37,69,'L'},
    {38,68,'L'},
    {40,67,'L'},
    {41,66,'L'},
    {42,65,'L'},
    {43,64,'L'},
    {44,63,'L'},
    {45,62,'L'},
    {46,61,'L'},
    {47,60,'L'},
    {49,59,'L'},
    {50,58,'L'},
    {51,57,'L'},
    {52,56,'L'},
    {53,55,'L'},
    {54,54,'L'},
    {55,53,'L'},
    {57,52,'L'},
    {58,51,'L'},
    {59,50,'L'},
    {60,49,'L'},
    {61,48,'L'},
    {62,47,'L'},
    {63,46,'L'},
    {65,45,'L'},
    {66,44,'L'},
    {67,43,'L'},
    {68,42,'L'},
    {69,41,'L'},
    {70,40,'L'},
    {71,39,'L'},
    {72,38,'L'},
    {73,37,'L'},
    {74,36,'L'},
    {75,35,'L'},
    {76,34,'L'},
    {77,33,'L'},
    {78,32,'L'},
    {79,31,'L'},
    {81,30,'L'},
    {82,29,'L'},
    {83,28,'L'},
    {84,27,'L'},
    {85,26,'L'},
    {86,25,'L'},
    {87,24,'L'},
    {89,23,'L'},
    {90,22,'L'},
    {91,21,'L'},
    {92,20,'L'},
    {93,19,'L'},
    {94,18,'L'},
    {95,17,'L'},
    {97,16,'L'},
    {98,15,'L'},
    {99,14,'L'},
    {100,13,'L'},
    {101,12,'L'},
    {102,11,'L'},
    {103,10,'L'},
    {105,9,'L'},
    {106,8,'L'},
    {107,7,'L'},
    {108,6,'L'},
    {109,5,'L'},
    {110,4,'L'},
    {113,3,'L'},
    {114,2,'L'},
    {115,1,'L'},
    {130,0,'S'},
    {144,1,'R'},
    {145,2,'R'},
    {146,3,'R'},
    {147,4,'R'},
    {148,5,'R'},
    {149,6,'R'},
    {150,7,'R'},
    {151,8,'R'},
    {153,9,'R'},
    {154,10,'R'},
    {155,11,'R'},
    {156,12,'R'},
    {157,13,'R'},
    {158,14,'R'},
    {159,15,'R'},
    {161,16,'R'},
    {162,17,'R'},
    {163,18,'R'},
    {164,19,'R'},
    {165,20,'R'},
    {167,21,'R'},
    {169,22,'R'},
    {170,23,'R'},
    {171,24,'R'},
    {172,25,'R'},
    {173,26,'R'},
    {174,27,'R'},
    {175,28,'R'},
    {177,29,'R'},
    {178,30,'R'},
    {179,31,'R'},
    {180,32,'R'},
    {181,33,'R'},
    {182,34,'R'},
    {183,35,'R'},
    {185,36,'R'},
    {186,37,'R'},
    {187,38,'R'},
    {188,39,'R'},
    {189,40,'R'},
    {190,41,'R'},
    {191,42,'R'},
    {193,43,'R'},
    {194,44,'R'},
    {195,45,'R'},
    {196,46,'R'},
    {197,47,'R'},
    {198,48,'R'},
    {199,49,'R'},
    {201,50,'R'},
    {202,51,'R'},
    {203,52,'R'},
    {204,53,'R'},
    {205,54,'R'},
    {206,55,'R'},
    {208,56,'R'},
    {209,57,'R'},
    {210,58,'R'},
    {211,59,'R'},
    {212,60,'R'},
    {213,61,'R'},
    {214,62,'R'},
    {215,63,'R'},
    {217,64,'R'},
    {218,65,'R'},
    {219,66,'R'},
    {220,67,'R'},
    {221,68,'R'},
    {222,69,'R'},
    {224,70,'R'},
    {225,71,'R'},
    {226,72,'R'},
    {227,73,'R'},
    {228,74,'R'},
    {229,75,'R'},
    {231,76,'R'},
    {232,77,'R'},
    {233,78,'R'},
    {234,79,'R'},
    {235,80,'R'},
    {236,81,'R'},
    {237,82,'R'},
    {239,83,'R'},
    {240,84,'R'},
    {241,85,'R'},
    {242,86,'R'},
    {243,87,'R'},
    {244,88,'R'},
    {245,89,'R'},
    {247,90,'R'},
    {248,91,'R'},
    {249,92,'R'},
    {250,93,'R'},
    {251,94,'R'},
    {252,95,'R'},
    {254,96,'R'},
    {255,97,'R'}
};

Slider_motor::Slider_motor(setResistorValueCallback setResistorCallback, u_int8_t sensor_PIN, bool initializeSensorPinForDigitalInput){
    if (InterruptableControlsArray::_activeControlsCount >= MAX_CONTROL_ITEMS){
        printf("Active interrupt devices count exceeded maximum number!");
        return;
    };
        
    InterruptableControlsArray::registerControl(this);


    this->_setResistorCallback = setResistorCallback;
    this->_control_activated   = false;     // accept changes to current settings, but don't send those settings to the motor!
    this->_sensor_PIN = sensor_PIN;

    if (initializeSensorPinForDigitalInput){
        gpio_init(this->_sensor_PIN);
        gpio_set_dir(this->_sensor_PIN, GPIO_IN);
    }

    InterruptableControlsArray::setIRQHandler(this->_sensor_PIN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);

    // calculate the size of settings array
    this->_settings_array_size = sizeof(AVAILABLE_CONTROLLER_SETTINGS_TABLE) / sizeof(strctControllerSetting_t);

    Slider_motor::stop();
}

void Slider_motor::activate(){
    this->_lastEncoded = 0; // it's very important to reset this buffer here!
    this->_turnback_in_progress = false;
    this->_control_activated = true;
    
    this->updateResistorValue();
}

void Slider_motor::deactivate(){
    this->_control_activated = false;
}

void Slider_motor::getSettingsIndexLimits(uint& min, uint& max){
    min = 0;
    max = this->_settings_array_size;
}


strctControllerSetting_t Slider_motor::getCurrentSetting(){
    return AVAILABLE_CONTROLLER_SETTINGS_TABLE[this->_current_setting_index];
}

u_int Slider_motor::getCurrentSettingIndex(){
    return this->_current_setting_index;
}

void Slider_motor::setCurrentSettingIndex(u_int settingIndex){
    if (settingIndex >= 0 && settingIndex < this->_settings_array_size){
        this->_current_setting_index = settingIndex;

        if (this->_control_activated) this->updateResistorValue();
    }
}

void Slider_motor::stop(){
    u_int stopSettingIndex = this->getSettingIndex(0, 'S');

    bool oldIsActivated = this->_control_activated;

    if (stopSettingIndex != ILLEGAL_MOTOR_SETTINGS_ARRAY_INDEX){
        this->_control_activated = true;

        // make sure this command will be sent to the motor even if controller is in deactivated state
        this->setCurrentSettingIndex(stopSettingIndex);

        this->_control_activated = oldIsActivated;
    }
}

void Slider_motor::setTurnBackOptions(uint64_t turnBackTime_us, enmSlider_Motor_Turnback_Speed_Curve speedCurve){
    this->_turnBackTime_us = turnBackTime_us;
    this->_speedCurve = speedCurve;
}

void Slider_motor::turnBack(){
    if (this->_turnback_in_progress) return;

    this->_turnback_in_progress = true;

    // remember start and end setting indexes
    char newDirection = AVAILABLE_CONTROLLER_SETTINGS_TABLE[this->_current_setting_index].direction == 'R' ? 'L' : 'R';
    uint8_t speed = AVAILABLE_CONTROLLER_SETTINGS_TABLE[this->_current_setting_index].speedValue;

    this->_turnback_start_setting_index = this->_current_setting_index;
    this->_turnback_final_setting_index = this->getSettingIndex(speed, newDirection);
    
    
    printf("start index: %d, end index: %d\n", this->_turnback_start_setting_index, this->_turnback_final_setting_index);

    // remember when the whole operation started 
    this->_turnback_time_started = time_us_64();
    
    if (this->_onStartTurnback) this->_onStartTurnback();
}

void Slider_motor::setOnStartTurnbackCallback(genericSliderMotorCallback cb){
    this->_onStartTurnback = cb;
}

void Slider_motor::setOnEndTurnbackCallback(genericSliderMotorCallback cb){
    this->_onEndTurnback = cb;
}

void Slider_motor::setOnUpdateControllerSettings(updateControllerSettingsCallback cb){
    this->_onUpdateControllerSettings = cb;
}


void Slider_motor::handleInterruptEvent(uint gpio, uint32_t events){
    if (gpio != this->_sensor_PIN || !this->_control_activated) return;

    int newValue = !gpio_get(this->_sensor_PIN);

    u_int8_t directionEncoded = AVAILABLE_CONTROLLER_SETTINGS_TABLE[this->_current_setting_index].direction == 'R' ? 0 : 1;   

    u_int8_t encoded = (newValue << 1) | directionEncoded;
    u_int8_t sum = ((this->_lastEncoded << 2) | encoded) & 0b00111111; // remember up to 3 state changes

    printf("SUM: " BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(sum));
    printf("\n");

    if (sum == 0b00000010){
        printf("poczatek zawracania prawego\n");
        this->_lastEncoded = sum;
        this->turnBack();        
    }
    else if (sum == 0b00000011){
        printf("poczatek zawracania lewego\n");
        this->_lastEncoded = sum;
        this->turnBack();
    }
    else if (sum == 0b00001001){
        printf("koniec zawracania prawego (bez wyjazdu ze znacznika)\n");
        this->_lastEncoded = 0;
    }
    else if (sum == 0b00001101 && this->_lastEncoded == 0b00100011){
        printf("koniec zawracania prawego\n");
        this->_lastEncoded = 0;
    }
    else if (sum == 0b00001100){
        printf("koniec zawracania lewego (bez wyjazdu ze znacznika)\n");
        this->_lastEncoded = 0;
    }
    else if (sum == 0b00011000){
        printf("koniec zawracania lewego\n");
        this->_lastEncoded = 0;
    }
    else {
        this->_lastEncoded = sum;
    }    
}

void Slider_motor::update(){
    if (this->_turnback_in_progress){
        // calculate which setting index to use now
        signed int end      = this->_turnback_final_setting_index;
        signed int start    = this->_turnback_start_setting_index;
        float indexDifference = end - start;

        // calculate how much time has passed since turnback started
        float time_passed = time_us_64() - this->_turnback_time_started;

        // is the time up?
        if (time_passed > this->_turnBackTime_us){
            this->setCurrentSettingIndex(_turnback_final_setting_index);
            this->_turnback_in_progress = false;

            if (this->_onEndTurnback) this->_onEndTurnback();

            if (this->_onUpdateControllerSettings) this->_onUpdateControllerSettings(this->getCurrentSetting());

            return;
        }

        // what is our time progress?
        float progress = time_passed / this->_turnBackTime_us;
                
        // convert progress to settings index
        float index_progress = 0;
        u_int      newIndex = 0;

        if (this->_speedCurve == linear){
            index_progress = indexDifference * progress;
            newIndex = this->_turnback_start_setting_index + index_progress;            
        }

        if (newIndex != this->_turnback_current_setting_index){            
            this->setCurrentSettingIndex(newIndex);

            this->_turnback_current_setting_index = newIndex;
        }
    }
}


/**
 *          PRIVATE
 */
u_int Slider_motor::getSettingIndex(uint8_t speed, char direction){
    for (int a = 0; a < this->_settings_array_size; a++){
        if (AVAILABLE_CONTROLLER_SETTINGS_TABLE[a].speedValue == speed && AVAILABLE_CONTROLLER_SETTINGS_TABLE[a].direction == direction) return a;
    }

    return ILLEGAL_MOTOR_SETTINGS_ARRAY_INDEX;
}

void  Slider_motor::updateResistorValue(){
    if (this->_setResistorCallback){
        this->_setResistorCallback(AVAILABLE_CONTROLLER_SETTINGS_TABLE[this->_current_setting_index].resistorValue);
    }
}