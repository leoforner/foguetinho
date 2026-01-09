#include <unity.h>

// Apenas compila no ambiente do Foguete (ESP-IDF)
#ifdef ESP_PLATFORM

#include <driver/i2c.h>
#include <esp_log.h>

// Configurações do GY-521 (Ajuste os pinos conforme seu hardware)
#define I2C_MASTER_SCL_IO 22 // GPIO Clock
#define I2C_MASTER_SDA_IO 21 // GPIO Data
#define I2C_MASTER_NUM 0     // Porta I2C 0
#define I2C_MASTER_FREQ_HZ 400000
#define MPU6050_ADDR 0x68 // Endereço padrão
#define MPU6050_WHO_AM_I_REG 0x75

void i2c_init_test()
{
    i2c_config_t conf = {};
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;

    // Instala o driver apenas se ainda não estiver instalado
    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
}

// Teste: Verifica se o MPU6050 está conectado e respondendo
void test_mpu6050_connection(void)
{
    uint8_t data = 0;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MPU6050_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, MPU6050_WHO_AM_I_REG, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MPU6050_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read_byte(cmd, &data, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, ret, "Falha na comunicacao I2C!");
    // O valor padrão do registro WHO_AM_I do MPU6050 é 0x68
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(0x68, data, "ID do Sensor Invalido! (Nao eh um MPU6050?)");
}

extern "C" void app_main()
{
    i2c_init_test(); // Inicializa hardware
    UNITY_BEGIN();
    RUN_TEST(test_mpu6050_connection);
    UNITY_END();
}
#endif