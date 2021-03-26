#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_system.h>
#include <bmp180.h>
#include <dht.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define SDA_GPIO 21
#define SCL_GPIO 22

extern char sensorsPayload[30];

void sensors_task(void *pvParameters)
{
    bmp180_dev_t dev;
    memset(&dev, 0, sizeof(bmp180_dev_t)); // Zero descriptor

    ESP_ERROR_CHECK_WITHOUT_ABORT(bmp180_init_desc(&dev, 0, SDA_GPIO, SCL_GPIO));
    ESP_ERROR_CHECK_WITHOUT_ABORT(bmp180_init(&dev));

    while (1)
    {

        /* bmp180 reading */
        float temp = -200;
        uint32_t pressure = -200;

        esp_err_t res = bmp180_measure(&dev, &temp, &pressure, BMP180_MODE_STANDARD);
        if (res != ESP_OK)
            printf("Could not read data from bmp 180 sensor: %d\n", res);
        else
            /* float is used in printf(). you need non-default configuration in
             * sdkconfig for ESP8266, which is enabled by default for this
             * example. see sdkconfig.defaults.esp8266
             */
            printf("Temperature: %.2f degrees Celsius; Pressure: %d Pa\n", temp, pressure);

        /* dht11 reading */
        static const dht_sensor_type_t sensor_type = DHT_TYPE_DHT11;
        static const gpio_num_t dht_gpio = 4;

        int16_t temperature = -2000;
        int16_t humidity = -2000;

        if (dht_read_data(sensor_type, dht_gpio, &humidity, &temperature) == ESP_OK) {
            printf("Humidity: %d%% Temp: %dC\n", humidity / 10, temperature / 10);
        }else {
                  printf("Could not read data from dht11 sensor\n");
        }
        //sprintf(sensorsPayload, "T2:%d," "H:%d", temperature/10, humidity/10);
        sprintf(sensorsPayload, "T1:%.2f," "P:%d," "T2:%d," "H:%d", temp, pressure, temperature/10, humidity/10);

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}