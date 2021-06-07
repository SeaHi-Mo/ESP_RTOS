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

struct CONTACT_INFORMATION{
	 char *phone_number;
	 char *address;
}; 
struct STUDENS{
	char *name;
	int sex;
	uint8_t age;
	struct CONTACT_INFORMATION information;
};

static void sendQueueTask(void *arg )
{
	char *QueueTAG="READ Queue MSG";
	struct STUDENS Studen;

	for(;;){

		if ( xQueueReceive(testQueueHandle,&Studen,portMAX_DELAY))
		{
			printf("%s=Stuen:\n",QueueTAG);
			printf("name   sex  age    phoneNumber      address\n");
			printf("%s   %s  %d     %s         %s\r\n",Studen.name,Studen.sex?"男":"女",Studen.age,Studen.information.phone_number,Studen.information.address);
		}
		vTaskDelay(10/portTICK_RATE_MS);

	}
}
void app_main()
{
	struct STUDENS Student={
		.name="梨花",
		.sex=1,
		.age=17,
		.information.phone_number="193xxx000",
		.information.address="北京市海淀区"
	};
	ESP_LOGI(TAG,"Studen:%s,%s,%d,%s,%s",Student.name,Student.sex?"男":"女",Student.age,Student.information.phone_number,Student.information.address);
	
	testQueueHandle=xQueueCreate(128,sizeof(struct STUDENS));

	xTaskCreate(sendQueueTask,"Send Queue Task",1024*2,NULL,10,NULL);
	while (1){
		xQueueSend(testQueueHandle,&Student,portMAX_DELAY);
		vTaskDelay(1000/portTICK_RATE_MS);
	}	
}
