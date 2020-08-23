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
#include "nvs_flash.h"

#include "Modbus_RTU.c"

/*
Definiciones 
*/
static const char *TAG = "uart_events";


void procesar_request(void* param);
void uart_task(void *param);
void blink_task(void *pvParameter);
struct MODBUS_SLAVE slave;

void app_main(void)
{
  




 //   xCreateQ
   esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
   
   
      //Iniciando un slave modbus
    xTaskCreatePinnedToCore(uart_task,"uart",60*1024,NULL,2,NULL,0);
   // xTaskCreatePinnedToCore(blink_task,"blink",4*1024,NULL,4,NULL,0);
   // xTaskCreatePinnedToCore(procesar_request,"procesar",40*1024,NULL,4,NULL,1);
    







	while(1)
	{
    //printf("esperando\n");
		vTaskDelay(1000);
	}
}



void uart_task(void *param)
{
    inicializarMODBUS_SLAVE(&slave,1, 2, 38400);   //id ,uart baudios

    cargar_registro(&slave,0, 4);
   cargar_registro(&slave,1, 3);
   cargar_registro(&slave,2, 8);
  
     printf("Se inicializo el micro \n");
  
   struct BUFFER request;
while(1)
{
           
      if(xQueueReceive(req_modbus,&request,200/portTICK_RATE_MS)==pdTRUE) 
        {
     
            procesarRequest(&request,&slave) ;
            
        }
       
        
       vTaskDelay(100);
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



void procesar_request(void* param)
{
    
  

}

