#include "./mcp425X.hpp"

MCP425X::MCP425X(spi_inst_t* spi_handler, uint chipSelectPIN, bool initializeCSPin){
    this->_spi = spi_handler;
    this->_chipSelectPIN = chipSelectPIN;

    if (initializeCSPin){
        // Chip select is active-low, so we'll initialise it to a driven-high state
        gpio_init(this->_chipSelectPIN);
        gpio_set_dir(this->_chipSelectPIN, GPIO_OUT);
        gpio_put(this->_chipSelectPIN, 1);

        // Make the CS pin available to picotool
        bi_decl(bi_1pin_with_name(this->_chipSelectPIN, "SPI CS"));
    }    
}

void MCP425X::sendCommand(uint8_t address, uint8_t value){
    // take the SS pin low to select the chip:
    gpio_put(this->_chipSelectPIN, 0);

    // send in the address and value via SPI:
    spi_write_blocking(this->_spi, &address, 1);
    spi_write_blocking(this->_spi, &value, 1);

    // take the SS pin high to de-select the chip:
    gpio_put(this->_chipSelectPIN, 1);
}