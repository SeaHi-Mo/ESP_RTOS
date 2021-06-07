#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/inet.h"
#include "esp_log.h"
#include "User_app_client.h"


#define HOST_IP_ADDR " "
#define PORT 8989
static const char *TAG="TCP CLIENT CONFIG";

static void tcpMessageHandler(void *arg)
{

}

void tcp_client_connect(void)
{
	struct sockaddr_in tcp_addr;
	int addr_family=0,ip_protocol=0;

	tcp_addr.sin_addr.s_addr=inet_addr(HOST_IP_ADDR);
	tcp_addr.sin_family=AF_INET;
	tcp_addr.sin_port=htons(PORT);
	addr_family=AF_INET;
	ip_protocol=IPPROTO_IP;
 //创建socket 套接字
	 int socket_fd=socket(addr_family,SOCK_STREAM,ip_protocol);	
 	if (socket_fd<0)
	{
		ESP_LOGI(TAG,"socket create fail：error %d\n",socket_fd);
		return -1;
	 }
	int res=connect(socket_fd,(struct socketaddr *)&tcp_addr,sizeof(struct socketaddr_in));
	if (res!=0)
	{
		ESP_LOGI(TAG,"connect fail: error %d\n",res);
		return -1;
	} else{
		ESP_LOGI(TAG,"connect successfully ip:%s  %d\n",tcp_addr.sin_addr.s_addr,tcp_addr.sin_port);	
		
	}
	
	
	
}
