/**    ___ _   __
 *    / _ \ | / /
 *   / // / |/ /
 *  /____/|___/
 *
 * @file main.h
 * @author Dominik Vagner, xvagne10
 * @brief Header file for a Heart Rate Monitor and Oximeter (MAX30102) for ESP32 platform using ESP-IDF.
 *        Results are displayed on an SSD1306 OLED display and can be accessed via a web server.
 * @date 03-12-2022
 */
/**
 * LIST OF REFERENCES
 *  Non-standard libraries used in this project:
 *   [1] https://github.com/nopnop2002/esp-idf-ssd1306
 *    ^^ This library is used for the display, and its example code was used as an inspiration.
 *    ^^ License: MIT
 *    ^^ Author: nopnop2002
 *   [2] https://github.com/JoshDumo/esp32-max30102
 *    ^^ This library is used for the MAX30102 sensor, and its example code was used as an inspiration.
 *    ^^ Author: Joshua D. John (JoshDumo)1
 *    ^^ The MAX30102 library was slightly adjusted/modified and calibrated to work better on my sensor.
 *  Documentation/datasheets:
 *   [3] https://datasheets.maximintegrated.com/en/ds/MAX30102.pdf
 *   [4] https://pdfserv.maximintegrated.com/en/an/AN6409.pdf
 *  Other references:
 *   [5] https://embeddedexplorer.com/esp32-wifi-access-point/
 *    ^^ This tutorial was used to set up ESP as a WiFi access point.
 *   [6] https://embeddedexplorer.com/esp32-web-server/
 *    ^^ This tutorial was used to set up a web server on the ESP.
 */ 

#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/i2c.h>
#include <esp_log.h>
#include <esp_system.h>
#include <esp_spi_flash.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_http_server.h>
#include <esp_netif.h>
#include <nvs_flash.h>
// Display Library ref.:[1]
#include "ssd1306.h"
// MAX30102 Library ref.:[2]
#include "max30102.h"

#define I2C_SCL_IO 22
#define I2C_SDA_IO 21
#define I2C_NUM I2C_NUM_0
#define I2C_FREQ_HZ 100000

// Global variables
static const char *TAG = "imp_21_xvagne10";
SSD1306_t screen;
max30102_config_t max30102_config = {};
char *last_bpm, *last_spo2;

// Function prototypes
/**
 * @brief Task for the sensor.
 * @param pvParameter
 */
void sensor_task(void *pvParameter);
/**
 * @brief Initialize the OLED display using SSD1306.
 * @return ESP_OK on success, ESP_FAIL on failure.
 */
esp_err_t display_init(void);
/**
 * @brief Initialize the I2C bus.
 * @return ESP_OK on success, ESP_FAIL on failure.
 */
esp_err_t sensor_i2c_init(void);
/**
 * @brief Initialize the MAX30102 sensor.
 * @return ESP_OK on success, ESP_FAIL on failure.
 */
esp_err_t sensor_init(void);
/**
 * @brief Initialize and configure the WiFi.
 * @return ESP_OK on success, ESP_FAIL on failure.
 */
esp_err_t wifi_init(void);
/**
 * @brief Initialize/setup the HTTP server. Library ref.:[5]
 * @return ESP_OK on success, ESP_FAIL on failure.
 */
esp_err_t http_init(void);
/**
 * @brief Handler for the index page. Library ref.:[6]
 * @param req
 * @return ESP_OK on success, ESP_FAIL on failure.
 */
esp_err_t index_handler(httpd_req_t *req);
/**
 * @brief Handler for the value page 
 * (endpoint returning a simple stirng with BPM and SpO2, used with AJAX).
 * @param req
 * @return ESP_OK on success, ESP_FAIL on failure.
 */
esp_err_t values_handler(httpd_req_t *req);

#endif // MAIN_H 