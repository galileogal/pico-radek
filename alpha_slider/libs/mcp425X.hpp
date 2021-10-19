#ifndef ___MCP425X_H___
#define ___MCP425X_H___

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"


/*
    Handler for MCP425X digital potentiometers
*/
class MCP425X {
    private:
        // SPI port handler
        spi_inst_t* _spi;
        uint _chipSelectPIN;

    public:
        // define addresses and commands
        static const uint8_t wiper0writeAddr = 0b00000000;
        static const uint8_t wiper1writeAddr = 0b00010000;
        static const uint8_t tconwriteAddr = 0b01000000;
        static const uint8_t tcon_0off_1on = 0b11110000;
        static const uint8_t tcon_0on_1off = 0b00001111;
        static const uint8_t tcon_0off_1off = 0b00000000;
        static const uint8_t tcon_0on_1on = 0b11111111;


        MCP425X(spi_inst_t* spi_handler, uint chipSelectPIN, bool initializeCSPin = false);

        // This function takes care of sending SPI data to the potentiometer
        void sendCommand(uint8_t address, uint8_t value);
};

#endif