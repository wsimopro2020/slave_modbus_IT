# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCS "esp32_lcd.c" "Modbus_RTU.c" "port_uart.c" "main.c")
set(COMPONENT_ADD_INCLUDEDIRS "")

register_component()
