#include "ota_setup.h"
#include "esp_log.h"
// #include "globals.h" // Não é mais necessário para o log_tag

static const ota_context_t* _ota_ctx = NULL;

void ota_init(const ota_context_t* context) {
    _ota_ctx = context;
    // Inicialização da partição OTA e configuração HTTP/HTTPS
    ESP_LOGI(_ota_ctx->log_tag, "OTA: Configuração inicializada");
}

void ota_check_updates(void) {
    // Lógica para verificar WiFi e baixar firmware
}