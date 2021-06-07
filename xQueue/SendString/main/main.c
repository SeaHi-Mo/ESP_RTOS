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
	char *Studen=NULL;
	for(;;){
		if ( xQueueReceive(testQueueHandle,&Studen,500/portTICK_RATE_MS) )
		{
			ESP_LOGI(QueueTAG,"MSG=%s",Studen);
		}
		vTaskDelay(10/portTICK_RATE_MS);
	}
}
void app_main()
{
	char *Student="lihua";
	ESP_LOGI(TAG,"START......");
	testQueueHandle=xQueueCreate(2,sizeof(Student));
	xTaskCreate(sendQueueTask,"Send Queue Task",1024*2,NULL,10,NULL);
	while (1){
		xQueueSend(testQueueHandle,&Student,500/portTICK_RATE_MS);
		vTaskDelay(1000/portTICK_RATE_MS);
	}	
}
