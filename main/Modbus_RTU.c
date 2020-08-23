/*
 Libreria modbus para dispositivos exclavos exclusivamente
*/


#include "Modbus_RTU.h"



void inicializarMODBUS_SLAVE(struct MODBUS_SLAVE* slave,uint8_t id, uint8_t uart, u_int16_t baudios)
{
    slave->ID =id;
    configurar_uart(&(slave->PORT),uart,baudios, PIN_RX_MODBUS, PIN_TX_MODBUS);
     inicializar_uart(&(slave->PORT));
    inicializarREGISTROS(&(slave->MAPA_MEM), CANTIDAD_ELEMENTOS , DIRECCION_INICIAL);   // INICIAS POR DEFECTO 16 REG EN DIRECION INICIAL 0
    borrarREGISTROS(&(slave->MAPA_MEM));                                               //INICIALIZAS EN CERO LA MEMORIA
}

/*
struct BUFFER response(struct BUFFER* request, struct MODBUS_SLAVE* slave)
{

}

9*/



void cargar_registro(struct MODBUS_SLAVE* slave,uint8_t direccion, uint16_t valor)
{
    //La direccion esta dentro del rango
    if(direccion >=(slave->MAPA_MEM.direccion_inicio)  && (direccion <= ((slave->MAPA_MEM.direccion_inicio)+(slave->MAPA_MEM.cant_elementos))))
    {
        slave->MAPA_MEM.datos[direccion-(slave->MAPA_MEM.direccion_inicio)]=valor;
    }
    else
    {
        printf("Error fuera de rango memoria\n");
    }
    slave->MAPA_MEM.direcciones_ocupadas = slave->MAPA_MEM.direcciones_ocupadas + 1 ;
}

void inicializarREGISTROS(struct REGISTROS_MEMORIA* registros,uint16_t cant_elementos, uint16_t direccion_inicio)
{
    registros->datos = (uint16_t*)malloc(sizeof(uint16_t)*cant_elementos);
    if(registros->datos != NULL)
    {
        registros->inicializado = 1;
        registros->cant_elementos= cant_elementos;
        registros->direccion_inicio = direccion_inicio;
        (registros->direcciones_ocupadas)=0;
    
    }

}



void borrarREGISTROS(struct REGISTROS_MEMORIA* registros)
{

    for(uint8_t i=0; i<(registros->cant_elementos); i++)
    {
        (registros->datos)[i]=0;
    }
    registros->direcciones_ocupadas=0;

}





/*
Calculo de CRC16
*/
void CRC16_2(uint8_t *buf, uint8_t len)
{  
  uint16_t crc = 0xFFFF;
  for (int pos = 0; pos < len; pos++)
  {
  crc ^= (uint16_t)buf[pos];    // XOR byte into least sig. byte of crc
  for (int i = 8; i != 0; i--) {    // Loop over each bit
    if ((crc & 0x0001) != 0) {      // If the LSB is set
      crc >>= 1;                    // Shift right and XOR 0xA001
      crc ^= 0xA001;
    }
    else                            // Else LSB is not set
      crc >>= 1;                    // Just shift right
    }
  }


  buf[len]=(uint8_t)crc;
  buf[len+1]=(uint8_t)(crc >> 8);
    
  
}


void agregarCRC_BUFFER(struct BUFFER* buffer)
{
   
    agregarCRC(buffer->buffer,buffer->size);
   //Aumento en 2 el tamaño del buffer, para contener al CRC
    printf("enviando respuesta\n");
     
   
    uart_write_bytes(2,(char*)buffer->buffer, (uint8_t)((buffer->size)));
     uart_flush(UART_NUM_2);


  






    
}


void agregarCRC(uint8_t* out,uint8_t len)
{
   uint8_t l= (len-2);
  uint16_t crc = 0xFFFF;
  for (int pos = 0; pos < l; pos++)
  {
  crc ^= (uint16_t)out[pos];    // XOR byte into least sig. byte of crc
  for (int i = 8; i != 0; i--) {    // Loop over each bit
    if ((crc & 0x0001) != 0) {      // If the LSB is set
      crc >>= 1;                    // Shift right and XOR 0xA001
      crc ^= 0xA001;
    }
    else                            // Else LSB is not set
      crc >>= 1;                    // Just shift right
    }
  }
  
//Agrego el CRC AL FINAL
  out[l]=(uint8_t)crc;
  out[l+1]=(uint8_t)(crc >> 8);
//  printf("calculadon crc..\n");

}

bool comprobarCRC(uint8_t* out,uint8_t len)
{
bool confirmacion=false;
if(len != 0)   //Si len es cero ignorar, error en la trama
{
        uint16_t crc = 0xFFFF;
        uint8_t l= len-2 ; //No DEBEMOS TENER EN CUENTA LOS 2 ULTIMOS BYTES
        //Calculo CRC con los elementos menos los 2 ultimos, son el CRC del mensaje a comprobar
        for (int pos = 0; pos < l; pos++)
    {
        crc ^= (uint16_t)out[pos];    // XOR byte into least sig. byte of crc
        for (int i = 8; i != 0; i--) {    // Loop over each bit
        if ((crc & 0x0001) != 0) {      // If the LSB is set
        crc >>= 1;                    // Shift right and XOR 0xA001
        crc ^= 0xA001;
        }
        else                            // Else LSB is not set
        crc >>= 1;                    // Just shift right
        }
    }
    //Verifico si los crc coincide
    if(out[l] == ((uint8_t)crc) && out[l+1]==((uint8_t)(crc >> 8)))
    {
          confirmacion=true;
    }
}
    
return confirmacion;

}




void procesarRequest(struct BUFFER* request,struct MODBUS_SLAVE* slave) 
{
    struct BUFFER response;
    if(request->size != 0)   //si size es cero ignoramos request mandamos error
    {
        printf("Estamos procesando una request\n");
        if(request->buffer[0]== slave->ID)
        {
         switch (request->buffer[1])   // Funciones soportadas
         {
         case 0x03:    //Funcion lectura de registros
            printf("La request es de funcion 3\n");
             procesarF3( request, &response, slave);
             agregarCRC_BUFFER(&response);

             break;

         case 0x10:   //Funcion de escritura de registros
            /* code */
            break;
         default:  //Cualquier otra funcion no soportada error
            
             break;
         }   
        }
        else
        {
            //ignorar, no es peticion para este dispositivo;
        }

        //RESPONDER
        
        //uart_read_bytes(2,(response.buffer),response.size,10);
        
    }
    else
    {
        //Ignorar no es trama modbus, ruido o algo mas
    }


}
/*


struct BUFFER
{
    uint8_t* buffer;
    uint8_t size;
};

*/
/*

struct REGISTROS_MEMORIA
{
    uint8_t inicializado;
    uint16_t* datos;
    uint16_t cant_elementos;
    uint16_t direccion_inicio;
    uint16_t direcciones_ocupadas;
    
};

*/

void procesarF3(struct BUFFER* req, struct BUFFER* response,struct MODBUS_SLAVE* slave)
{

    uint16_t memorias[20]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
    response->size=3+((uint8_t)req->buffer[5])*2+2;  //el ultimo +2 es por el CRC
        response->buffer = (uint8_t*)malloc(response->size);
        response->buffer[0]=slave->ID ; //ID
        response->buffer[1]=req->buffer[1] ;  //FUNCION
        response->buffer[2]=((uint8_t)req->buffer[5])*2; //numero de bytes enviados     bytes=2*cantidad de registros pedidos
        //registros------>primero el byte alto y despues el bytes low
         uint16_t direccionrequerida =  ((uint16_t)(req->buffer[2])<<8) |((uint16_t) req->buffer[3]);
        uint16_t offset = direccionrequerida -slave->MAPA_MEM.direccion_inicio;
        printf("el offset es:%d\n",offset);
        if(offset >=0)
        {
            printf("dentro de procesamiento, offset legal\n");
             uint16_t cantidad_memorias =  ((uint16_t)(req->buffer[4])<<8) |((uint16_t) req->buffer[5]);
            
             if((slave->MAPA_MEM.cant_elementos - offset) >   cantidad_memorias)  
            {
                for(uint16_t k=0,i=0, j=3 ; i<(cantidad_memorias*2);i++)    //recordemos las memorias van en el buffer primero byte hight
                {
               //         printf("copiando memorias...\n");
                    response->buffer[j+i]=(uint8_t)(slave->MAPA_MEM.datos[offset+k]>>8);
                    response->buffer[j+(i+1)]=(uint8_t)(slave->MAPA_MEM.datos[offset+k]);
                    //     response->buffer[j+i]=(uint8_t)(memorias[offset+k]>>8);
                    //    response->buffer[j+(i+1)]=(uint8_t)(memorias[offset+k]);
                        k++;
                        i++;  //necesito que i avance 2 lugares
                }
            }   
            else
            {
              getERROR_BUFFER(response,slave);
            }
            
        }
        else
        {
             getERROR_BUFFER(response,slave);
           //error memoria fuera de rango    (ejemplo: dir inicial: 4  , dir mem pedida 2---fallo )
        }
       


}






struct BUFFER getError(uint8_t funcion,uint8_t id)   //erro solo lleva id y codigo generico junto con crc
{
    struct BUFFER error;
    error.buffer = (uint8_t*)malloc(3);
    error.buffer[0]=id;
    error.size = 3;

        switch (funcion)
        {
        case 0x03:    //funcion lectura de registros
            error.buffer = (uint8_t*)malloc(3);
            error.buffer[1]=0x83;  //codigo de error
            error.buffer[2]=0x01;  // uno de las excepciones , valido 1,2,3 y 4
            break;
        case 0x10: //Escribir multiples registros
            error.buffer[1]=0x90 ; //codigo de error
            error.buffer[2]=0x01 ; // uno de las excepciones , valido 1,2,3 y 4
            break;

        default:  //Escribir codigo de error de funcion indefinida
            break;
        }
    return error;
}


//Peticiones en escritura tiene 6 bytes minimo
//peticiones en lectura tienen 5 bytes al menos

//deben reenviarse estos bytes con el correspondiente codigo de error

void getERROR_BUFFER(struct BUFFER* buffer,struct MODBUS_SLAVE* slave)

{
    getError(buffer->buffer,slave->ID);
}

/*

    /Generando un error
          response.buffer =(uint8_t*)malloc();
          response.buffer[0]=0x83  //codigo de error
          response.buffer[1]=0x01  // uno de las excepciones , valido 1,2,3 y 4

*/

