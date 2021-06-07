#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_log.h"

static const char *TAG="xQueue Test";
static xQueueHandle testQueueHandle=NULL;

static void sendQueueTask(void *arg )
{
	char *QueueTAG="READ Queue MSG";
	int i[10]={0},j=0;

	for(;;){
		if ( xQueueReceive(testQueueHandle,(const void*)i,500/portTICK_RATE_MS) )
		{
			printf("MSG=[");
			for(j=0;i[j]!='\0';j++){
				printf("%d,",i[j]);
			}
			printf("]\r\n");
		}
		vTaskDelay(10/portTICK_RATE_MS);
	}
}

void app_main()
{
	int i[10]={1,2,3,4,5,6,7,8,9};
	ESP_LOGI(TAG,"START......");
	testQueueHandle=xQueueCreate(2,sizeof(i));
	xTaskCreate(sendQueueTask,"Send Queue Task",1024*2,NULL,10,NULL);
	while (1){
		xQueueSend(testQueueHandle,(const void*)i,500/portTICK_RATE_MS);
		vTaskDelay(1000/portTICK_RATE_MS);
		//i++;
	}	
}
