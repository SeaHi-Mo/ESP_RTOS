#ifndef _OLED_DEVICE_H
#define _OLED_DEVICE_H

#define I2C_MASTER_SDA_IO 5
#define I2C_MASTER_SCL_IO 6

#define OLED_SlveAddress 0x3C

#define OLED_Data       0x40  //写数据       
#define OLED_Cmd        0x00 //写命令
#define OLED_Brightnss  0xCF //OLED背景光的亮度

#define OLED_Size 96     //OLED 尺寸设置：91 为0.91英寸，96为0.96英寸

#if OLED_Size == 91
#define OLED_X_Width 	128
#define OLED_Y_Width 	32
#define OLED_SIZE 	 	0x1F  //屏幕大小设置 1F 为1/32 3F为 1/64
#define OLED_RSLT 		0X02  //分辨率 128 *32
#elif OLED_Size == 96
#define OLED_X_Width 	128
#define OLED_Y_Width 	64
#define OLED_SIZE		0x3F
#define OLED_RSLT 		0X12  //分辨率 128 *64
#endif
#define I2C_MASTER_NUM 1

extern SemaphoreHandle_t oled_int_mutex; 
esp_err_t oled_write_data(i2c_port_t i2c_num,uint16_t data);
esp_err_t oled_write_cmd(i2c_port_t i2c_num,uint16_t oled_cmd);
void oled_set_pos(i2c_port_t i2c_num,unsigned int x,unsigned int y);
void oled_set_fill(i2c_port_t i2c_num,unsigned char bmp_data);
void oled_clear_display(i2c_port_t i2c_num);
void oled_init(i2c_port_t i2c_num);
void OLED_Printfch(i2c_port_t i2c_num,unsigned char x,unsigned char y,unsigned char ch);
void OLED_PrintfStr(i2c_port_t i2c_num,unsigned char x, unsigned char y,unsigned char *ch);
void OLED_P16x16Ch(i2c_port_t i2c_num,unsigned char x, unsigned char y, unsigned char N);
#endif
