/* 
UART Interrupt Example
*/
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_intr_alloc.h"


/*
Definiciones 
*/
static const char *TAG = "uart_events";

#define BLINK_GPIO GPIO_NUM_2


#define EX_UART_NUM UART_NUM_0
#define PATTERN_CHR_NUM    (3)         /*!< Set the number of consecutive and identical characters received by receiver which defines a UART pattern*/

#define BUF_SIZE (1024)
#define RD_BUF_SIZE (BUF_SIZE)
static QueueHandle_t uart0_queue;

// Both definition are same and valid
//static uart_isr_handle_t *handle_console;
static intr_handle_t handle_console;

// Receive buffer to collect incoming data
uint8_t rxbuf[256];
// Register to collect data length
uint16_t urxlen;

#define NOTASK 0











void configurarUart2( uint32_t baud,uint8_t bitstop, uint8_t paridad, uint8_t rx , uint8_t tx );    //18 y 19
void putStringUart2(const char* string);









/*
 * Define UART interrupt subroutine to ackowledge interrupt   para UART2
 */
static void IRAM_ATTR uart_intr_handle(void *arg)
{
  uint16_t rx_fifo_len, status;
  uint16_t i=0;
  
  status = UART2.int_st.val; // read UART interrupt Status
  rx_fifo_len = UART2.status.rxfifo_cnt; // read number of bytes in UART buffer
  uint16_t size=rx_fifo_len;
  while(rx_fifo_len){
   rxbuf[i++] = UART2.fifo.rw_byte; // read all bytes
   rx_fifo_len--;
 }
  
 // after reading bytes from buffer clear UART interrupt status
 uart_clear_intr_status(2, UART_RXFIFO_FULL_INT_CLR|UART_RXFIFO_TOUT_INT_CLR);

// a test code or debug code to indicate UART receives successfully,
// you can redirect received byte as echo also
 //uart_write_bytes(EX_UART_NUM, (const char*) rxbuf, 10);
 printf("Se recibio : %s\n-----------------------------\n",(const char*) rxbuf);
 rxbuf[size]=0;
 
//  putStringUart2((char*)rxbuf);
//  putStringUart2("\n");
 
 //printf("recibimos mensaje :%s\n",rxbuf);

}








void blink_task(void *pvParameter);

void app_main(void)
{

    
	xTaskCreate(blink_task, "blink_task", 1024, NULL, 5, NULL);
  //   configurarUart2( 115200,1, 0, 18 , 19 ) ;
   int ret;
	esp_log_level_set(TAG, ESP_LOG_INFO);

	/* Configure parameters of an UART driver,
	* communication pins and install the driver */
	uart_config_t uart_config = {
		.baud_rate = 115200,
		.data_bits = UART_DATA_8_BITS,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE
	};

	ESP_ERROR_CHECK(uart_param_config(2, &uart_config));

	//Set UART log level
	esp_log_level_set(TAG, ESP_LOG_INFO);

	//Set UART pins (using UART0 default pins ie no changes.)
	ESP_ERROR_CHECK(uart_set_pin(2, rx, tx, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

	//Install UART driver, and get the queue.
	ESP_ERROR_CHECK(uart_driver_install(2, BUF_SIZE * 2, 0, 0, NULL, 0));

	// release the pre registered UART handler/subroutine
	ESP_ERROR_CHECK(uart_isr_free(2));

	// register new UART subroutine
	ESP_ERROR_CHECK(uart_isr_register(2,uart_intr_handle, NULL, ESP_INTR_FLAG_IRAM, &handle_console));

	// enable RX interrupt
	ESP_ERROR_CHECK(uart_enable_rx_intr(2));

	while(1)
	{
		vTaskDelay(1000);
	}
}













void blink_task(void *pvParameter)
{
    gpio_pad_select_gpio(BLINK_GPIO);
    
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    
    while(1) {
        /* Blink off (output low) */
        gpio_set_level(BLINK_GPIO, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        /* Blink on (output high) */
        gpio_set_level(BLINK_GPIO, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}





void configurarUart2( uint32_t baud,uint8_t bitstop, uint8_t paridad, uint8_t rx , uint8_t tx )    //18 y 19
{

    QueueHandle_t uart_queue;

    uart_config_t uart_config =
  {
    .baud_rate =baud,
    .data_bits = UART_DATA_8_BITS,
    .parity= paridad,
    .stop_bits =bitstop,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,    //UART_HW_FLOWCTRL_DISABLE    UART_HW_FLOWCTRL_CTS_RTS
    
  };

ESP_ERROR_CHECK(uart_param_config(UART_NUM_2,&uart_config));
ESP_ERROR_CHECK(uart_set_pin(UART_NUM_2,rx,tx,UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE));   //18 Y 19     , 17 Y 18   RX Y TX
ESP_ERROR_CHECK(uart_driver_install(UART_NUM_2,(const int)2048,(const int)2048,1000,&uart_queue,0));
}

void putStringUart2(const char* string)
{
 uart_write_bytes(UART_NUM_2,(const char*)string, strlen(string));   
}
