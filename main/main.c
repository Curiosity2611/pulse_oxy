#include <stdio.h>
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "max30100.h"

#define SDA_GPIO    25
#define SCL_GPIO    26
#define I2C_FREQ    100000
#define I2C_PORT    I2C_NUM_0

uint8_t temp_int_data;
uint8_t temp_frac_data;
uint8_t temp_measured;
uint8_t read_temp_int_reg = MAX_30100_TEMP_INT_VALUE;
uint8_t read_temp_frac_reg = MAX_30100_TEMP_FRAC_VALUE;

uint8_t data;
uint8_t read_data_addr = 0x01;
uint8_t en_temp_readings = MAX_30100_MODE_CONFIG_REG;
uint8_t writeData = MAX_30100_EN_TEMP;

void app_main(void)
{

    i2c_config_t i2c_config = 
    {
    .mode = I2C_MODE_MASTER,
    .sda_io_num = SDA_GPIO,
    .scl_io_num = SCL_GPIO,
    .sda_pullup_en = GPIO_PULLUP_ENABLE,
    .scl_pullup_en = GPIO_PULLUP_ENABLE,
    .master.clk_speed = I2C_FREQ
    };

    i2c_param_config(I2C_PORT, &i2c_config);
    i2c_driver_install(I2C_PORT, I2C_MODE_MASTER, 0, 0, 0);

    //write
    i2c_cmd_handle_t cmd_handle1 = i2c_cmd_link_create();
    i2c_master_start(cmd_handle1);
    i2c_master_write_byte(cmd_handle1, MAX30100_DEVICE_ADDR << 1 | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd_handle1, en_temp_readings, true);

    i2c_master_write_byte(cmd_handle1, writeData, true);
    i2c_master_stop(cmd_handle1);

    i2c_master_cmd_begin(I2C_PORT, cmd_handle1, 1000 /portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd_handle1);

    printf("Temp En has been written\n");

    while (1)
    {
    i2c_cmd_handle_t cmd_handle = i2c_cmd_link_create();
    i2c_master_start(cmd_handle);
    i2c_master_write_byte(cmd_handle, MAX30100_DEVICE_ADDR << 1 | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd_handle, read_temp_int_reg, true);
    i2c_master_start(cmd_handle);

    i2c_master_write_byte(cmd_handle, MAX30100_DEVICE_ADDR << 1 | I2C_MASTER_READ, true);

    i2c_master_read(cmd_handle, (uint8_t *)&temp_int_data, 1, I2C_MASTER_ACK);
    i2c_master_read(cmd_handle, (uint8_t *)&temp_frac_data, 1, I2C_MASTER_ACK);
    i2c_master_stop(cmd_handle);

    i2c_master_cmd_begin(I2C_PORT, cmd_handle, 1000 /portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd_handle);
    printf("The temp_int in hex is: 0x%x\n",temp_int_data);

    temp_measured = temp_int_data + temp_frac_data;

    printf("The temp in hex is: %d\n",temp_measured);
    printf("*********************************************\n");
        vTaskDelay(2000 / portTICK_RATE_MS);
    }
    
}
