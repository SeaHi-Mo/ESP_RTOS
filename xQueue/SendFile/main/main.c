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
static struct QUEUE_FILE{
	char *name;
	char *file_data;
	uint8_t file_size;
};

static void sendQueueTask(void *arg )
{
	char *QueueTAG="READ Queue MSG";
	struct QUEUE_FILE *_file;

	for(;;){
	
		if ( xQueueReceive(testQueueHandle,&_file,portMAX_DELAY) )
		{
			printf("file name:%s,%d,%s\n",_file->name,_file->file_size,_file->file_data);
			
		}
		vTaskDelay(10/portTICK_RATE_MS);
	}
}
void app_main()
{

	struct QUEUE_FILE *__file;
	struct QUEUE_FILE *_file;
	__file=(struct QUEUE_FILE *)malloc(sizeof(struct QUEUE_FILE *));
	_file=(struct QUEUE_FILE *)malloc(sizeof(struct QUEUE_FILE *));

	__file->name="Hell";
	__file->file_size=102;
	__file->file_data="Hi";
	
	_file->name="hell";
	_file->file_size=96;
	_file->file_data="hi";

	ESP_LOGI(TAG,"START......");

	testQueueHandle=xQueueCreate(3,sizeof(struct QUEUE_FILE *));
		
	xTaskCreate(sendQueueTask,"Send Queue Task",1024*2,NULL,10,NULL);
	while (1){
		xQueueSendToFront(testQueueHandle,&__file,portMAX_DELAY);
		xQueueSendToBack(testQueueHandle,&_file,portMAX_DELAY);	
		xQueueSendToBack(testQueueHandle,&__file,portMAX_DELAY);	
		printf("Send Over\n");	
		vTaskDelay(1000/portTICK_RATE_MS);		
	}
	free(__file);	
	free(_file);
}
