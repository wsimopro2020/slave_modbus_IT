/*
CONFIGURACIONES PORT UART PARA DISPOSITIVO
*/

#include "port_uart.h"

static QueueHandle_t request_Modbus;
static QueueHandle_t size_request;
static QueueHandle_t req_modbus;

static QueueHandle_t flag_request;




// Both definition are same and valid
//static uart_isr_handle_t *handle_console;
static intr_handle_t handle_console;


struct BUFFER req;








static void IRAM_ATTR uart_intr_handle(void *arg)
{


 
  
  uint16_t rx_fifo_len, status;
 
  uint16_t i=0;
  
  status = UART2.int_st.val; // read UART interrupt Status
  rx_fifo_len = UART2.status.rxfifo_cnt; // read number of bytes in UART buffer
   req.size =(uint8_t)rx_fifo_len;   //solo baffer para 255 elementos
   req.buffer= (uint8_t*)malloc(rx_fifo_len);
  while(rx_fifo_len){
   req.buffer[i++] = UART2.fifo.rw_byte; // read all bytes
   rx_fifo_len--;
 }
    xQueueSendToBackFromISR(req_modbus, &req,100/portTICK_RATE_MS);

  uart_clear_intr_status(2, UART_RXFIFO_FULL_INT_CLR|UART_RXFIFO_TOUT_INT_CLR);
  //uart_disable_rx_intr(2);  //Deshabilito las interrupciones






}






void configurar_uart(struct PORT_UART* port,uint8_t uart, u_int16_t baudios, uint8_t rx, uint8_t tx)
{
        port->uart=uart;
        port->baudios=baudios;
        port->rx = rx;
        port->tx= tx;
}





void inicializar_uart(struct PORT_UART* port)
{     
/*
UART_PARITY_DISABLE = 0x0

    Disable UART parity

UART_PARITY_EVEN = 0x2

    Enable UART even parity

UART_PARITY_ODD = 0x3
*/


	uart_config_t uart_config = {
		.baud_rate = port->baudios,
		.data_bits = UART_DATA_8_BITS,
		.parity = 2,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE
	};

	ESP_ERROR_CHECK(uart_param_config(2, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(port->uart,port->rx, port->tx, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
	//Install UART driver, and get the queue.


  //configuraciones adicioneles

  //uart_set_tx_idle_num(2, 2); //2 uart, 2 tiempo de bits experando


	ESP_ERROR_CHECK(uart_driver_install(port->uart, bufferModbusSlave, 0, 0, NULL, 0));  //buffer tx no usado, jode interrupciones,

	// release the pre registered UART handler/subroutine
  	ESP_ERROR_CHECK(uart_isr_free(port->uart));
	// register new UART subroutine
	ESP_ERROR_CHECK(uart_isr_register(port->uart,uart_intr_handle, NULL, ESP_INTR_FLAG_IRAM, &handle_console));
  
	// enable RX interrupt
	ESP_ERROR_CHECK(uart_enable_rx_intr(port->uart));
//  ESP_ERROR_CHECK(uart_disable_tx_intr(port->uart));
//INICIALIZANDO LA COLA PARA RECIBIR REQUEST
  //  request_Modbus=xQueueCreate(5,bufferModbusSlave);
   // size_request = xQueueCreate(5,sizeof(uint8_t));
  
    req_modbus =  xQueueCreate(1,sizeof(struct BUFFER));
   flag_request = xQueueCreate(1,sizeof(bool));
   
}



/*
Importante rutina de interrupcion para recibir tramas modbus y enviarlas a Queue

DEFINICION SOLO VALIDA PARA UART2 ,modificar linea 66 en caso de cambiar la uart

*/

