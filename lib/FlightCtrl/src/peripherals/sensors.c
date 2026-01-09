#include "sensors.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "math.h"

#define TAG_SENSORS "SENSORS"

// Configurações I2C
#define I2C_MASTER_SCL_IO           22
#define I2C_MASTER_SDA_IO           21
#define I2C_MASTER_NUM              0
#define I2C_MASTER_FREQ_HZ          400000

void sensors_init(void) {
    // 1. Configuração do Driver I2C
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
        .clk_flags = 0
    };
    
    ESP_ERROR_CHECK(i2c_param_config(I2C_MASTER_NUM, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0));

    // Aqui você inicializaria MPU6050 / BMP280 usando esp-idf-lib
    
    ESP_LOGI(TAG_SENSORS, "Sensores I2C Inicializados");
}

void sensors_read(TelemetryPacket* data) {
    // Simulação de leitura (Substituir pelos drivers reais)
    
    // Exemplo de preenchimento
    data->accel_z = 9.81; // 1G simulado
    data->angle_pitch = 0.0; 
    data->altitude = 0.0; // Ler do barômetro
}

void sensors_calibrate(void) {
    ESP_LOGI(TAG_SENSORS, "Calibrando Giroscópio...");
}