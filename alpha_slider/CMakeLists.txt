set(projname "alpha_slider")

project(${projname})

add_executable(${projname}
                alpha_slider.cpp
        )

target_sources(${projname} PRIVATE
        libs/lcd_i2c.cpp 
        libs/lcd_i2c.hpp
        libs/cd4051controller.hpp
        libs/cd4051controller.cpp
        libs/mcp425x.hpp
        libs/mcp425x.cpp
        libs/rotary_encoder.hpp
        libs/rotary_encoder.cpp
        libs/button_wcallback.hpp
        libs/button_wcallback.cpp
        libs/blinking_led.hpp
        libs/blinking_led.cpp
        libs/interruptable/interruptable_controls_array.hpp
        libs/interruptable/interruptable_controls_array.cpp
        libs/interruptable/interruptable_control.hpp
        libs/interruptable/interruptable_control.cpp
        libs/slider_motor/slider_motor.hpp
        libs/slider_motor/slider_motor.cpp
        libs/IR_barcode_scanner/IR_barcode_scanner.hpp
        libs/IR_barcode_scanner/IR_barcode_scanner.cpp        
)

# Pull in our pico_stdlib which aggregates commonly used features
target_link_libraries(alpha_slider pico_stdlib hardware_i2c hardware_adc hardware_spi)

# create map/bin/hex file etc.
pico_add_extra_outputs(alpha_slider)


pico_enable_stdio_usb(alpha_slider 1)
pico_enable_stdio_uart(alpha_slider 0)