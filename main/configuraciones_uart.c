







void configurarUart( void )    //18 y 19
{

    QueueHandle_t uart_queue;

    uart_config_t uart_config =
  {
    .baud_rate =115200,
    .data_bits = UART_DATA_8_BITS,
    .parity= 0,
    .stop_bits =1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,    //UART_HW_FLOWCTRL_DISABLE    UART_HW_FLOWCTRL_CTS_RTS
    
  };

ESP_ERROR_CHECK(uart_param_config(UART_NUM_0,&uart_config));
ESP_ERROR_CHECK(uart_set_pin(UART_NUM_0,UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE));   //18 Y 19     , 17 Y 18   RX Y TX
ESP_ERROR_CHECK(uart_driver_install(UART_NUM_0,(const int)2048,(const int)2048,1000,&uart_queue,0));
}
