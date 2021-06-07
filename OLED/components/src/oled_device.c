#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_err.h"
#include "driver/i2c.h"
#include "oled_device.h"
#include "esp_log.h"
#include "codetab.h"

#define I2C_MASTER_TX_BUF_DISABLE 0            /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0            /*!< I2C master doesn't need buffer */
#define I2C_MASTER_FREQ_HZ 100000

#define ACK_CHECK_EN 0x1
#define ACK_CHECK_DID 0X0
#define Font_Size 16

static const char *TAG="OLED_DEVICE";

SemaphoreHandle_t oled_int_mutex =NULL; 

//i2c 初始化配置
static esp_err_t i2c_master_init_config(void) 
{
	i2c_port_t i2c_master_port = I2C_NUM_0;
	
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
		
        .clk_flags = 0,          /*!< Optional, you can use I2C_SCLK_SRC_FLAG_* flags to choose i2c source clock here. */
    };
	
    esp_err_t err = i2c_param_config(i2c_master_port, &conf);
    if (err != ESP_OK) {
        return err;
    }
	err=i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
	ESP_LOGI(TAG,"i2c init config:%s",err==ESP_OK?"ESP_OK":"ESP_FAIL");
    return err;
}
//OLED 写数据
esp_err_t oled_write_data(i2c_port_t i2c_num,uint16_t data)
{
	i2c_cmd_handle_t cmd=i2c_cmd_link_create();
	int ret=0;
	
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd,(OLED_SlveAddress<<1)|I2C_MASTER_WRITE,ACK_CHECK_EN);
	i2c_master_write_byte(cmd,OLED_Data,ACK_CHECK_EN);
	i2c_master_write_byte(cmd,data,ACK_CHECK_EN);
	i2c_master_stop(cmd);

 	ret=i2c_master_cmd_begin(i2c_num, cmd,1000/portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);

	return ret;
}
//OLED 写命令
esp_err_t oled_write_cmd(i2c_port_t i2c_num,uint16_t oled_cmd)
{
	i2c_cmd_handle_t cmd=i2c_cmd_link_create();
	
	esp_err_t ret=0;

	i2c_master_start(cmd);
	i2c_master_write_byte(cmd,(OLED_SlveAddress<<1) | I2C_MASTER_WRITE,true);
	i2c_master_write_byte(cmd,OLED_Cmd,true);
	i2c_master_write_byte(cmd,oled_cmd,true);
	i2c_master_stop(cmd);
	
	ret=i2c_master_cmd_begin(i2c_num,cmd,1000/portTICK_RATE_MS);

	ESP_LOGI(TAG,"i2C master_cmd_begin:%d\n",ret);
	
	i2c_cmd_link_delete(cmd);
		
	return ret;
}
//OLED 设置光标
void oled_set_pos(i2c_port_t i2c_num,unsigned int x,unsigned int y)
{
   	oled_write_cmd(i2c_num,0xb0+y);
  	oled_write_cmd(i2c_num,((x&0xf0)>>4)|0x10);
    oled_write_cmd(i2c_num,(x&0x0f)|0x01);
}
//设置全屏显示
void oled_set_fill(i2c_port_t i2c_num,unsigned char bmp_data)
{
    unsigned char y,x;

    for (y = 0; y < 8; y++)
    {
        ESP_ERROR_CHECK(oled_write_cmd(i2c_num,0xb0+y));
        ESP_ERROR_CHECK(oled_write_cmd(i2c_num,0x01));
        ESP_ERROR_CHECK(oled_write_cmd(i2c_num,0x10));
        for (x = 0; x < OLED_X_Width; x++)
        ESP_ERROR_CHECK( oled_write_data(i2c_num,bmp_data));
    }
 }
//OLED清屏
void __attribute__((unused))oled_clear_display(i2c_port_t i2c_num)
{
   unsigned char y,x;

    for (y = 0; y  < 8; y++)
    {
        ESP_ERROR_CHECK(oled_write_cmd(i2c_num,0xb0+y));
        ESP_ERROR_CHECK(oled_write_cmd(i2c_num,0x01));
        ESP_ERROR_CHECK(oled_write_cmd(i2c_num,0x10));
        for (x = 0; x < OLED_X_Width; x++)
         ESP_ERROR_CHECK( oled_write_data(i2c_num,0));
    } 

}
//OLED初始化
void oled_init(i2c_port_t i2c_num)
{
	i2c_master_init_config();
    vTaskDelay(100/portTICK_RATE_MS);//这个延时非常重要
	
	ESP_ERROR_CHECK(oled_write_cmd(i2c_num,0xae));//--turn off oled panel
	ESP_ERROR_CHECK(oled_write_cmd(i2c_num,0x00));//---set low column address
	ESP_ERROR_CHECK(oled_write_cmd(i2c_num,0x10));//---set high column address
	ESP_ERROR_CHECK(oled_write_cmd(i2c_num,0x40));//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	
	ESP_ERROR_CHECK(oled_write_cmd(i2c_num,0x81));//--set contrast control register 对比度设置
	ESP_ERROR_CHECK(oled_write_cmd(i2c_num,OLED_Brightnss)); // Set SEG Output Current Brightness 设置亮度
		
	ESP_ERROR_CHECK(oled_write_cmd(i2c_num,0xa1));//--Set SEG/Column Mapping     0xa0×ó??·??? 0xa1????
	ESP_ERROR_CHECK(oled_write_cmd(i2c_num,0xc8));//Set COM/Row Scan Direction   0xc0????·??? 0xc8????
	ESP_ERROR_CHECK(oled_write_cmd(i2c_num,0xa6));//--set normal display
	ESP_ERROR_CHECK(oled_write_cmd(i2c_num,0xa8));//--set multiplex ratio(1 to 64)
	ESP_ERROR_CHECK(oled_write_cmd(i2c_num,OLED_SIZE));//--1/64 duty
	ESP_ERROR_CHECK(oled_write_cmd(i2c_num,0xb3));//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	ESP_ERROR_CHECK(oled_write_cmd(i2c_num,0x00));//-not offset
	ESP_ERROR_CHECK(oled_write_cmd(i2c_num,0xd5));//--set display clock divide ratio/oscillator frequency
	ESP_ERROR_CHECK(oled_write_cmd(i2c_num,0x80));//--set divide ratio, Set Clock as 100 Frames/Sec
	ESP_ERROR_CHECK(oled_write_cmd(i2c_num,0xd9));//--set pre-charge period
	ESP_ERROR_CHECK(oled_write_cmd(i2c_num,0xf1));//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock

	ESP_ERROR_CHECK(oled_write_cmd(i2c_num,0xda));//--set com pins hardware configuration
	ESP_ERROR_CHECK(oled_write_cmd(i2c_num,OLED_RSLT));//设置分辨率 02为 128*32 12为128*64

	ESP_ERROR_CHECK(oled_write_cmd(i2c_num,0xdb));//--set vcomh
	ESP_ERROR_CHECK(oled_write_cmd(i2c_num,0x40));//Set VCOM Deselect Level
	ESP_ERROR_CHECK(oled_write_cmd(i2c_num,0x20));//-Set Page Addressing Mode (0x00/0x01/0x02)
	ESP_ERROR_CHECK(oled_write_cmd(i2c_num,0x02));//

	ESP_ERROR_CHECK(oled_write_cmd(i2c_num,0x8d));//--set Charge Pump enable/disable
	ESP_ERROR_CHECK(oled_write_cmd(i2c_num,0x14));//--set(0x10) disable
	ESP_ERROR_CHECK(oled_write_cmd(i2c_num,0xa4));// Disable Entire Display On (0xa4/0xa5)
	ESP_ERROR_CHECK(oled_write_cmd(i2c_num,0xa6));// Disable Inverse Display On (0xa6/a7) 
	ESP_ERROR_CHECK(oled_write_cmd(i2c_num,0xaf));//--turn on oled panel

	ESP_ERROR_CHECK(oled_write_cmd(i2c_num,0xaf));//--turn on oled panel	
	oled_set_fill(i2c_num,0x00); //初始化清屏
	oled_set_pos(i2c_num,0,0); 

	ESP_LOGI(TAG,"Init OK");
}
 /**************显示一组 8*6 的标准ASCLL码字符 ，x,y 为位置，y的范围为 0~7 ****************/
void OLED_Printfch(i2c_port_t i2c_num,unsigned char x,unsigned char y,unsigned char ch)
{
	unsigned char c=0,i=0;
	c=ch-' ';
  if(x>128-1){x=0;y+=1;}
   if(Font_Size==16)
	 {
		 oled_set_pos(i2c_num,x,y); 
		 for(i=0;i<8;i++)
		 ESP_ERROR_CHECK(oled_write_data(i2c_num,F8X16[c*16+i]));
		 oled_set_pos(i2c_num,x,y+1); 
		 for(i=0;i<8;i++)
		  ESP_ERROR_CHECK(oled_write_data(i2c_num,F8X16[c*16+i+8]));
	 }		 
	 else {
			oled_set_pos(i2c_num,x,y); 
		    for(i=0;i<6;i++)
			ESP_ERROR_CHECK(oled_write_data(i2c_num,F6x8[c][i])); 
	 }
}
/*******************显示一组 8*16 大小的标准ASCLL码 ，x,y 为坐标，y的范围 为 0~7 ****************/
void OLED_PrintfStr(i2c_port_t i2c_num,unsigned char x,unsigned char y,unsigned char *ch)
{
	unsigned int i=0;
	ESP_LOGI(TAG,"Display: %d,%d,%s",x,y,ch);
	while(ch[i]!='\0')
	{
	  	OLED_Printfch(i2c_num,x,y,ch[i]);
		x+=8;
		if(x>120){x=0;y+=1;}
		i++;
	}
}
/*****************显示一个16*16 的点阵，x,y 为坐标，y 的范围为 0~7 ,N为第几个字****************************/

void OLED_P16x16Ch(i2c_port_t i2c_num,unsigned char x, unsigned char y, unsigned char N)
{
	unsigned char wm=0;
	unsigned int adder=32*N;
	oled_set_pos(i2c_num,x,y);
	for(wm = 0;wm < 16;wm++)
	{
		oled_write_data(i2c_num,F16x16[adder]);	
		adder += 1;
	}
	oled_set_pos(i2c_num,x,y+1);
	for(wm = 0;wm < 16;wm++)
	{
		oled_write_data(i2c_num,F16x16[adder]);
		adder += 1;
	} 	  	
}