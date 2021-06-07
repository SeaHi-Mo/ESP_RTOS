#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_log.h"

#include "protocol_examples_common.h"
#include "User_app_client.h"

static const char *TAG="TCP_Client";

void app_main()
{
	hello();
	while (1){
		
		vTaskDelay(1);
	}	
}
