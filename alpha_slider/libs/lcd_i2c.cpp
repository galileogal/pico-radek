#include "./lcd_i2c.hpp"
#include <string.h>

Screen::Screen(i2c_inst_t *i2c, unsigned char address, bool oneline, uint8_t charsize)
{
    this->_i2c = i2c;
    this->_address = address;


    this->_disp_func = this->LCD_DISPLAYON; // | 0x10
    if (!oneline){
        this->_disp_func |= this->LCD_2LINE;
    }
    else if (charsize){
        // for 1-line displays you can choose another dotsize
        this->_disp_func |= this->LCD_5x10DOTS;
    }
        

    // wait for display init after power-on
    sleep_ms(50); // 50ms

    
    // send function set
    this->cmd(this->LCD_FUNCTIONSET | this->_disp_func);

    sleep_us(4500); // 4.5ms

    this->cmd(this->LCD_FUNCTIONSET | this->_disp_func);

    sleep_us(150); // 150µs = 0.15ms

    this->cmd(this->LCD_FUNCTIONSET | this->_disp_func);
    this->cmd(this->LCD_FUNCTIONSET | this->_disp_func);

    // turn on the display
    this->_disp_ctrl = this->LCD_DISPLAYON | this->LCD_CURSOROFF | this->LCD_BLINKOFF;
    this->display(true);

    // clear it
    this->clear();

    // set default text direction (left-to-right)
    this->_disp_mode = this->LCD_ENTRYLEFT | this->LCD_ENTRYSHIFTDECREMENT;
    this->cmd(this->LCD_ENTRYMODESET | this->_disp_mode);
    
}

void Screen::cmd(uint8_t command)
{
    uint8_t data[2] = {0x80, command};

    i2c_write_blocking(this->_i2c, this->_address, 0, 0, false);
    i2c_write_blocking(this->_i2c, this->_address, data, 2, false);
}

void Screen::display(bool state){
    if (state)
    {
        this->_disp_ctrl |= this->LCD_DISPLAYON;
        this->cmd(this->LCD_DISPLAYCONTROL  | this->_disp_ctrl);
    }        
    else
    {
        this->_disp_ctrl &= ~this->LCD_DISPLAYON;
        this->cmd(this->LCD_DISPLAYCONTROL | this->_disp_ctrl);
    }
}

void Screen::clear()
{
    this->cmd(this->LCD_CLEARDISPLAY);
    sleep_ms(2); // 2ms
}


void Screen::write_char(uint8_t c)
{
    uint8_t data[2] = {0x40, c};

    i2c_write_blocking(this->_i2c, this->_address, 0, 0, false);
    i2c_write_blocking(this->_i2c, this->_address, data, 2, false);
}

void Screen::write(const char* displayText)
{
    char buffer[100];
    strcpy(&buffer[1], displayText);
    buffer[0] = 0x40;

    i2c_write_blocking(this->_i2c, this->_address, 0, 0, false);
    i2c_write_blocking(this->_i2c, this->_address, (uint8_t*)&buffer, (unsigned)strlen(buffer), false);
}

void Screen::cursor(bool state)
{
    if (state)
    {
        this->_disp_ctrl |= this->LCD_CURSORON;
        this->cmd(this->LCD_DISPLAYCONTROL  | this->_disp_ctrl);
    }        
    else
    {
        this->_disp_ctrl &= ~this->LCD_CURSORON;
        this->cmd(this->LCD_DISPLAYCONTROL  | this->_disp_ctrl);
    }        
}

void Screen::setCursor(uint8_t col, uint8_t row)
{
    col = !row ? col | 0x80 : col | 0xc0;
    this->cmd(col);
}

void Screen::autoscroll(bool state)
{
    if (state)
    {
        this->_disp_ctrl |= this->LCD_ENTRYSHIFTINCREMENT;
        this->cmd(this->LCD_DISPLAYCONTROL  | this->_disp_ctrl);
    }        
    else
    {
        this->_disp_ctrl &= ~this->LCD_ENTRYSHIFTINCREMENT;
        this->cmd(this->LCD_DISPLAYCONTROL  | this->_disp_ctrl);
    }        
}

void Screen::blink(bool state)
{
    if (state)
    {
        this->_disp_ctrl |= this->LCD_BLINKON;
        this->cmd(this->LCD_DISPLAYCONTROL  | this->_disp_ctrl);
    }        
    else
    {
        this->_disp_ctrl &= ~this->LCD_BLINKON;
        this->cmd(this->LCD_DISPLAYCONTROL  | this->_disp_ctrl);
    }        
}

void Screen::home()
{
    this->cmd(this->LCD_RETURNHOME);
    sleep_ms(2); // 2m
}











Backlight::Backlight(i2c_inst_t* i2c, uint8_t address)
{
    this->_i2c = i2c;
    this->_address = address;

    // initialize
    this->set_register(this->REG_MODE1, 0);
    this->set_register(this->REG_MODE2, 0);

    // all LED control by PWM
    this->set_register(this->REG_OUTPUT, 0xAA);
}

void Backlight::blinkLed()
{
    this->set_register(0x07, 0x17); //# blink every seconds
    this->set_register(0x06, 0x7f); //# 50% duty cycle
}

void Backlight::set_register(uint8_t addr, uint8_t value)
{
    uint8_t data[2] = {addr, value};

    i2c_write_blocking(this->_i2c, this->_address, 0, 0, false);
    i2c_write_blocking(this->_i2c, this->_address, data, 2, false);
}

void Backlight::set_color(uint8_t red, uint8_t green, uint8_t blue)
{
    this->set_register(this->REG_RED, red);
    this->set_register(this->REG_GREEN, green);
    this->set_register(this->REG_BLUE, blue);
}


Display::Display(i2c_inst_t* i2c, uint8_t lcd_addr, uint8_t rgb_addr)
{
    this->_backlight = new Backlight(i2c, rgb_addr);
    this->_screen = new Screen(i2c, lcd_addr);
}

void Display::color(uint8_t r, uint8_t g, uint8_t b)
{
    this->_backlight->set_color(r, g, b);
}

void Display::write(const char* displayText)
{
    this->_screen->write(displayText); 
}

void Display::move(uint8_t col, uint8_t row)
{
    this->_screen->setCursor(col, row);
}

void Display::cursor(bool state)
{
    this->_screen->cursor(state); 
}

void Display::blink(bool state)
{
    this->_screen->blink(state);
}

void Display::blinkLed()
{
    this->_backlight->blinkLed();
}

void Display::autoscroll(bool state)
{
    this->_screen->autoscroll(state);
}

void Display::display(bool state)
{
    this->_screen->display(state);
}

void Display::clear()
{
    this->_screen->clear();
}

void Display::home()
{
    this->_screen->home();
}