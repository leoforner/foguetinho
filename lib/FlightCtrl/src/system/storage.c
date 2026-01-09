#include "storage.h"
#include "esp_spiffs.h"
#include "esp_log.h"
#include <stdio.h>

#define TAG_STORAGE "STORAGE"
#define LOG_FILE_PATH "/spiffs/flight_log.bin"

void storage_init(void) {
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true
    };

    // Registra e monta o SPIFFS
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG_STORAGE, "Falha ao montar ou formatar sistema de arquivos");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG_STORAGE, "Partição SPIFFS não encontrada");
        } else {
            ESP_LOGE(TAG_STORAGE, "Falha ao inicializar SPIFFS (%s)", esp_err_to_name(ret));
        }
        return;
    }
    ESP_LOGI(TAG_STORAGE, "SPIFFS Montado com sucesso");
}

int storage_log(const TelemetryPacket* data) {
    FILE* f = fopen(LOG_FILE_PATH, "ab"); // 'ab' = append binary
    if (f == NULL) {
        return -1;
    }
    fwrite(data, sizeof(TelemetryPacket), 1, f);
    fclose(f);
    return 0;
}