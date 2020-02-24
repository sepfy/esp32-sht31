/* 
  sht31 example
*/
#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "sdkconfig.h"
#include "sht31.h"

static const char *TAG = "sht31 example";

void app_main(void)
{  
    float temp, humi;
    ESP_ERROR_CHECK(sht31_init());
    while(true) {
      ESP_ERROR_CHECK(sht31_read_temp_humi(&temp, &humi));

      ESP_LOGI(TAG, "temp = %.2f, humi = %.2f\n", temp, humi);
      vTaskDelay(2000 / portTICK_PERIOD_MS); 
    }

}
