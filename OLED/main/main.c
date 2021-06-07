#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/i2c.h"
#include "oled_device.h"

void app_main()
{
	i2c_port_t i2c_num=I2C_NUM_0;
	char *str="Hello world!";
	oled_init(i2c_num);
	OLED_PrintfStr(i2c_num,0,0,(unsigned char *)str);
	OLED_P16x16Ch(i2c_num,0,2,0);	
	OLED_P16x16Ch(i2c_num,16,2,1);

	while(1){

		vTaskDelay(10/portTICK_RATE_MS);
	}
}
