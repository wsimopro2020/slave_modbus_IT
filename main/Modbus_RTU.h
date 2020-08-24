/*

Libreria modbus para dispositivos en modo exclavo esclusivamente
Autor : German Gabriel Velardez
*/

#include <stdio.h>
#include "port_uart.c"   //definiciones de puerto uart

#include "esp_task_wdt.h"

#define ID_SLAVE_MODBUS 1   //modificar en caso de ser necesario
#define BLINK_GPIO GPIO_NUM_2    //Led para presencia y funcionamiento
/*
Estructuras Modbus
*/

#define CANTIDAD_ELEMENTOS 20
#define DIRECCION_INICIAL 0


enum ESTADOS_SLAVE {
    
    
    PROCESANDO=1,
    ESPERANDO,
    ERROR ,
};

struct REGISTROS_MEMORIA
{
    uint8_t inicializado;
    uint16_t* datos;
    uint16_t cant_elementos;
    uint16_t direccion_inicio;
    uint16_t direcciones_ocupadas;
    
};


static struct MODBUS_SLAVE
{
      uint8_t  ESTADO;
    uint8_t  ID;
    struct PORT_UART PORT;

    struct   REGISTROS_MEMORIA MAPA_MEM;
  
    QueueHandle_t request_Modbus; 
    

};




void inicializarMODBUS_SLAVE(struct MODBUS_SLAVE* slave,uint8_t ID, uint8_t PORT, u_int16_t BAUDIOS);

void inicializarREGISTROS(struct REGISTROS_MEMORIA* registros,uint16_t cant_elementos, uint16_t direccion_inicio);

void borrarREGISTROS(struct REGISTROS_MEMORIA* registros);

struct BUFFER response(struct BUFFER* request, struct MODBUS_SLAVE* slave);

void CRC16_2(uint8_t *buf, uint8_t len);
void agregarCRC_BUFFER(struct BUFFER* buffer);
bool comprobarCRC(uint8_t* out,uint8_t len);
void agregarCRC(uint8_t* out,uint8_t len);
void getError(uint8_t funcion,struct BUFFER* error_response,uint8_t id) ; //erro solo lleva id y codigo generico junto con crc
void getERROR_BUFFER(struct BUFFER* request,struct BUFFER* response,struct MODBUS_SLAVE* slave);
void procesarF16(struct BUFFER* req, struct BUFFER* response,struct MODBUS_SLAVE* slave);
void procesarF3(struct BUFFER* req, struct BUFFER* response,struct MODBUS_SLAVE* slave);
void cargar_registro(struct MODBUS_SLAVE* slave,uint8_t direccion, uint16_t valor);
/*
Recibo la request y empiezo a desmenuzar por partes

void getERROR_BUFFER(struct BUFFER* buffer)


//Write multiple register funcion: 16(0x10)
// /funcion/start address/ quantity register/byte count/ value.../


*/


/*
Leer multiples registros

/id/funcion/start addres/ cantidad de registros   ----> siempre primero H luego Low
*/


void procesarRequest(struct BUFFER* request,struct MODBUS_SLAVE* slave) ;


