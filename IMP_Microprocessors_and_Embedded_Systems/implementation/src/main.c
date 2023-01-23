/**    ___ _   __
 *    / _ \ | / /
 *   / // / |/ /
 *  /____/|___/
 *
 * @file main.c
 * @author Dominik Vagner, xvagne10
 * @brief Main file for a Heart Rate Monitor and Oximeter (MAX30102) for ESP32 platform using ESP-IDF.
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
 *    ^^ Author: Joshua D. John (JoshDumo)
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

#include "main.h"
#include "index.h"

void app_main(void) {
    ESP_LOGI(TAG, "Starting application...");
    ESP_LOGI(TAG, "Initializing components/sensors...");
    ESP_ERROR_CHECK(display_init());
    ESP_ERROR_CHECK(sensor_i2c_init());
    ESP_ERROR_CHECK(sensor_init());

    ESP_LOGI(TAG, "Initializing WiFi and HTTP server...");
    ESP_ERROR_CHECK(wifi_init());
    ESP_ERROR_CHECK(http_init());
    ESP_LOGI(TAG, "Initialization complete.");

    ESP_LOGI(TAG, "Starting main task!");
    xTaskCreate(&sensor_task, "sensor_task", 4096, NULL, 1, NULL);
}

void sensor_task(void *pvParameter) {
    max30102_data_t sensor_data = {};
    for (;;) {
        ESP_ERROR_CHECK(max30102_update(&max30102_config, &sensor_data));
        if (sensor_data.pulse_detected) {
            char *pulse, *spo2;
            size_t pulse_len, spo2_len;
            // Convert the pulse and SpO2 values to strings.
            pulse_len = snprintf(NULL, 0, "BPM: %.2f", sensor_data.heart_bpm);
            pulse = malloc(pulse_len + 1);
            last_bpm = malloc(pulse_len + 1);
            snprintf(pulse, pulse_len + 1, "BPM: %.2f", sensor_data.heart_bpm);
            snprintf(last_bpm, pulse_len + 1, "BPM: %.2f", sensor_data.heart_bpm);
            spo2_len = snprintf(NULL, 0, "SpO2: %.2f", sensor_data.spO2);
            spo2 = malloc(spo2_len + 1);
            last_spo2 = malloc(spo2_len + 1);
            snprintf(spo2, spo2_len + 1, "SpO2: %.2f", sensor_data.spO2);
            snprintf(last_spo2, spo2_len + 1, "SpO2: %.2f", sensor_data.spO2);
            // Display the values on the display.
            ssd1306_clear_line(&screen, 0, false);
            ssd1306_clear_line(&screen, 1, false);
            ssd1306_display_text(&screen, 0, pulse, pulse_len, false);
            ssd1306_display_text(&screen, 1, spo2, spo2_len, false);
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

esp_err_t display_init(void) {
    spi_master_init(&screen, CONFIG_MOSI_GPIO, CONFIG_SCLK_GPIO, CONFIG_CS_GPIO, CONFIG_DC_GPIO, CONFIG_RESET_GPIO);
    ssd1306_init(&screen, 128, 64);
    ssd1306_clear_screen(&screen, false);
    ssd1306_contrast(&screen, 0xFF);
    if (ssd1306_get_width(&screen) <= 0 || ssd1306_get_height(&screen) <= 0) {
        ESP_LOGE(TAG, "Display initialization failed.");
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t sensor_i2c_init(void) {
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = I2C_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_FREQ_HZ;
    conf.clk_flags = 0;
    i2c_param_config(I2C_NUM, &conf);
    return i2c_driver_install(I2C_NUM, conf.mode, 0, 0, 0);
}

esp_err_t sensor_init(void) {
    return max30102_init(
        &max30102_config,
        I2C_NUM,
        MAX30102_DEFAULT_OPERATING_MODE,
        MAX30102_SAMPLING_RATE_100HZ,
        MAX30102_PULSE_WIDTH_200US_ADC_13,
        MAX30102_LED_CURRENT_11MA,
        MAX30102_LED_CURRENT_11MA,
        MAX30102_DEFAULT_MEAN_FILTER_SIZE,
        MAX30102_DEFAULT_PULSE_BPM_SAMPLE_SIZE,
        MAX30102_DEFAULT_ADC_RANGE,
        MAX30102_SAMPLE_AVERAGING_1,
        MAX30102_DEFAULT_ROLL_OVER,
        MAX30102_ALMOST_FULL_15,
        false
    );
}

esp_err_t wifi_init(void) {
    ESP_ERROR_CHECK(nvs_flash_init());
    // Initialize the WiFi driver.
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // Configure the WiFi driver.
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = CONFIG_ESP_WIFI_SSID,
            .ssid_len = strlen(CONFIG_ESP_WIFI_SSID),
            .channel = CONFIG_ESP_WIFI_CHANNEL,
            .password = CONFIG_ESP_WIFI_PASSWORD,
            .max_connection = CONFIG_ESP_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };
    if (strlen(CONFIG_ESP_WIFI_PASSWORD) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    return ESP_OK;
}

esp_err_t http_init(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;
    ESP_ERROR_CHECK(httpd_start(&server, &config));
    httpd_uri_t index_uri = {
        .uri      = "/",
        .method   = HTTP_GET,
        .handler  = index_handler,
        .user_ctx = NULL
    };
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &index_uri));
    httpd_uri_t values_uri = {
        .uri      = "/values",
        .method   = HTTP_GET,
        .handler  = values_handler,
        .user_ctx = NULL
    };
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &values_uri));
    return ESP_OK;
}

esp_err_t index_handler(httpd_req_t *req) {
    httpd_resp_send(req, main_page, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t values_handler(httpd_req_t *req) {
    char *resp = malloc(strlen(last_bpm) + strlen(last_spo2) + 2);
    snprintf(resp, strlen(last_bpm) + strlen(last_spo2) + 2, "%s_%s", last_bpm, last_spo2);
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}
