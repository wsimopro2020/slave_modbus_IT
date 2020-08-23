/*
DEFINICIONES DE PUERTO UART
*/

#include <stdio.h>
#include "driver/uart.h"

#define UART_MODBUS UART_NUM_2
#define BAUDIOS_PREDEF 19200
#define PIN_RX_MODBUS   18
#define PIN_TX_MODBUS   19



#define bufferModbusSlave 256   //256 Tamaño maximo de peticion modbus



struct PORT_UART
{
    uint8_t uart;
    uint16_t baudios;
    uint8_t rx;
    uint8_t tx;
    
};


struct BUFFER
{
    uint8_t* buffer;
    uint8_t size;
};

/*
Configuraciones rapidas para uart 
*/

void configurar_uart(struct PORT_UART* port,uint8_t uart, u_int16_t baudios, uint8_t rx, uint8_t tx);
void inicializar_uart(struct PORT_UART* port);
