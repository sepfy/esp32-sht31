#include "esp_log.h"
#include "driver/i2c.h"

static i2c_port_t i2c_port = I2C_NUM_0;

#define SDA_IO_NUM 21;
#define SCL_IO_NUM 22;

#define I2C_MASTER_TX_BUF_DISABLE 0 
#define I2C_MASTER_RX_BUF_DISABLE 0
#define WRITE_BIT I2C_MASTER_WRITE            
#define READ_BIT I2C_MASTER_READ
#define ACK_CHECK_EN 0x1
#define ACK_CHECK_DIS 0x0
#define ACK_VAL 0x0
#define NACK_VAL 0x1


static esp_err_t sht31_init(void)
{
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = SDA_IO_NUM;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = SCL_IO_NUM;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = 1000000;
    i2c_param_config(i2c_port, &conf);
    return i2c_driver_install(i2c_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

static uint8_t sht31_crc(uint8_t *data) {

    uint8_t crc = 0xff;
    int i, j;
    for(i = 0; i < 2; i++) {
        crc ^= data[i];
        for(j = 0; j < 8; j++) {
            if(crc & 0x80) {
                crc <<= 1;
                crc ^= 0x131;
            }
            else
                crc <<= 1;
        }
    }
    return crc;
}

static esp_err_t sht31_read_temp_humi(float *temp, float *humi)
{
    // See http://wiki.seeedstudio.com/Grove-TempAndHumi_Sensor-SHT31/
    // Write 0x00 to address 0x24
    unsigned char data_wr[] = {0x24, 0x00};
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (0x44 << 1) | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write(cmd, data_wr, sizeof(data_wr), ACK_CHECK_EN);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_port, cmd, 1000 / portTICK_RATE_MS);
    if (ret != ESP_OK) {
        return ret;
    }
    i2c_cmd_link_delete(cmd);

    // Delay 20 ms
    vTaskDelay(2);

    // Read 6 bytes
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (0x44 << 1) | READ_BIT, ACK_CHECK_EN);
    size_t size = 6;
    uint8_t *data_rd = malloc(size);
    if (size > 1) {
        i2c_master_read(cmd, data_rd, size - 1, ACK_VAL);
    }
    i2c_master_read_byte(cmd, data_rd + size - 1, NACK_VAL);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(i2c_port, cmd, 1000 / portTICK_RATE_MS);
    if (ret != ESP_OK) {
        return ret;
    }
    i2c_cmd_link_delete(cmd);


    // check error
    if(data_rd[2] != sht31_crc(data_rd) || 
       data_rd[5] != sht31_crc(data_rd + 3)) 
        return ESP_ERR_INVALID_CRC;
    
    *temp = -45 + (175 *(float)(data_rd[0] * 256 + data_rd[1])/ 65535.0);
    *humi = 100 * (float)(data_rd[3] * 256 + data_rd[4]) / 65535.0;

    return ESP_OK;
}
