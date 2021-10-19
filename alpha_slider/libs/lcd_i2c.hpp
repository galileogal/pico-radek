#ifndef ___LCD_I2C_H___
#define ___LCD_I2C_H___

#include "pico/stdlib.h"
#include "hardware/i2c.h"

class Screen
{
    private:
        i2c_inst_t*     _i2c;
        uint8_t   _address;
        uint8_t   _disp_func;
        uint8_t   _disp_ctrl;
        uint8_t   _disp_mode;

    public:
        // commands
        static const uint8_t LCD_CLEARDISPLAY = 0x01;
        static const uint8_t LCD_RETURNHOME = 0x02;
        static const uint8_t LCD_ENTRYMODESET = 0x04;
        static const uint8_t LCD_DISPLAYCONTROL = 0x08;
        static const uint8_t LCD_CURSORSHIFT = 0x10;
        static const uint8_t LCD_FUNCTIONSET = 0x20;
        static const uint8_t LCD_SETCGRAMADDR = 0x40;
        static const uint8_t LCD_SETDDRAMADDR = 0x80;

        // flags for display entry mode
        static const uint8_t LCD_ENTRYRIGHT = 0x00;
        static const uint8_t LCD_ENTRYLEFT = 0x02;
        static const uint8_t LCD_ENTRYSHIFTINCREMENT = 0x01;
        static const uint8_t LCD_ENTRYSHIFTDECREMENT = 0x00;

        // flags for display on/off control
        static const uint8_t LCD_DISPLAYON = 0x04;
        static const uint8_t LCD_DISPLAYOFF = 0x00;
        static const uint8_t LCD_CURSORON = 0x02;
        static const uint8_t LCD_CURSOROFF = 0x00;
        static const uint8_t LCD_BLINKON = 0x01;
        static const uint8_t LCD_BLINKOFF = 0x00;

        // flags for display/cursor shift
        static const uint8_t LCD_DISPLAYMOVE = 0x08;
        static const uint8_t LCD_CURSORMOVE = 0x00;
        static const uint8_t LCD_MOVERIGHT = 0x04;
        static const uint8_t LCD_MOVELEFT = 0x00;

        // flags for function set
        static const uint8_t LCD_8BITMODE = 0x10;
        static const uint8_t LCD_4BITMODE = 0x00;
        static const uint8_t LCD_2LINE = 0x08;
        static const uint8_t LCD_1LINE = 0x00;
        static const uint8_t LCD_5x10DOTS = 0x04;
        static const uint8_t LCD_5x8DOTS = 0x00;

    
    Screen(i2c_inst_t *i2c, uint8_t address, bool oneline = false, uint8_t charsize = LCD_5x8DOTS);

    void cmd(uint8_t command);
    void display(bool state);
    void clear();
    void write_char(uint8_t c);
    void write(const char* displayText);
    void cursor(bool state);
    void setCursor(uint8_t col, uint8_t row);
    void autoscroll(bool state);
    void blink(bool state);
    void home();
};

class Backlight
{
    private:
        i2c_inst_t*     _i2c;
        uint8_t   _address;

    public:

        static const uint8_t REG_RED = 0x04; // # pwm2
        static const uint8_t REG_GREEN = 0x03; // # pwm1
        static const uint8_t REG_BLUE = 0x02; // # pwm0

        static const uint8_t REG_MODE1 = 0x00;
        static const uint8_t REG_MODE2 = 0x01;
        static const uint8_t REG_OUTPUT = 0x08;



    Backlight(i2c_inst_t* i2c, uint8_t address);

    void blinkLed();
    void set_register(uint8_t addr, uint8_t value);
    void set_color(uint8_t red, uint8_t green, uint8_t blue);
};

class Display
{
    private:
        Backlight*   _backlight;
        Screen*      _screen;

    public:

        Display(i2c_inst_t* i2c, uint8_t lcd_addr=62, uint8_t rgb_addr=96);
        
        void color(uint8_t r, uint8_t g, uint8_t b);
        void write(const char* displayText);
        void move(uint8_t col, uint8_t row);
        void cursor(bool state);
        void blink(bool state);
        void blinkLed();
        void autoscroll(bool state);
        void display(bool state);
        void clear();
        void home();
};

#endif