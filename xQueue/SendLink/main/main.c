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


typedef struct STUDENTS{
	char *name;
	char success;
	uint16_t ID;
	
	struct STUDENTS *next;
}_students;

_students *crartorAddStudentsLink(_students *Handle,_students *new)
{
	_students *temp=NULL;
    temp=Handle;
	if (Handle==NULL)
		return new;
	while (temp->next!=NULL)	
		temp=temp->next;	
	temp->next=new;

	return Handle;
}

static void sendQueueTask(void *arg )
{
	char *QueueTAG="READ Queue MSG";
	_students *Handle=NULL;
	for(;;){
		if ( xQueueReceive(testQueueHandle,&Handle,500/portTICK_RATE_MS) )
		{
			while(Handle!=NULL){
				printf("name:%s  ID：%d  success:%d\r\n",Handle->name,Handle->ID,Handle->success);		
				Handle=Handle->next;
			}
		}
		vTaskDelay(10/portTICK_RATE_MS);
	}
}

void app_main()
{
	_students *Handle=NULL;

	_students STU1={
		.name="lihua",
		.ID=1,
		.success=103,
	};

	_students STU2={
		.name="zhangsan",
		.ID=2,
		.success=135,
	};

	_students STU3={
		.name="lisi",
		.ID=3,
		.success=200,
	};
	//把结构体加入链表当中
	Handle=crartorAddStudentsLink(Handle,&STU1);
	Handle=crartorAddStudentsLink(Handle,&STU2);
	Handle=crartorAddStudentsLink(Handle,&STU3);

	ESP_LOGI(TAG,"START......");	
	testQueueHandle=xQueueCreate(2,sizeof(Handle));
	xTaskCreate(sendQueueTask,"Send Queue Task",1024*2,NULL,10,NULL);

	while (1){
		xQueueSend(testQueueHandle,&Handle,portMAX_DELAY);
		vTaskDelay(1000/portTICK_RATE_MS);
	}	
}
