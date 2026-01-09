#include "espnow_setup.h"
#include "esp_now.h"
#include "esp_log.h"
#include "string.h"
#include "RocketShared.h"
#include "mac_addresses.h" // centralized MAC addresses

static QueueHandle_t _uplink_queue = NULL;
static const char* _log_tag = "ESPNOW"; // Tag padrão

// Callback de recebimento (Processa comandos)
static void on_data_recv(const uint8_t * mac, const uint8_t *incomingData, int len) {
    if (len == sizeof(UplinkPacket)) {
        UplinkPacket packet;
        memcpy(&packet, incomingData, sizeof(UplinkPacket));

        // Envia para a fila (Não bloqueante)
        if (_uplink_queue != NULL) {
            xQueueSendFromISR(_uplink_queue, &packet, NULL);
        }
    }
}

esp_err_t espnow_init_custom(const espnow_context_t* context) {
    if (context == NULL || context->uplink_queue == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    // Configura as variáveis estáticas baseadas no contexto injetado
    _uplink_queue = context->uplink_queue;
    if (context->log_tag != NULL) {
        _log_tag = context->log_tag;
    }

    esp_err_t ret = esp_now_init();
    if (ret != ESP_OK) {
        ESP_LOGE(_log_tag, "Falha ao inicializar ESP-NOW: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // 1. Configura PMK (Criptografia)
    ret = esp_now_set_pmk((const uint8_t *)ROCKET_PMK);
    if (ret != ESP_OK) {
        ESP_LOGE(_log_tag, "Falha ao configurar PMK: %s", esp_err_to_name(ret));
        return ret;
    }

    // 2. Registra Callback
    ret = esp_now_register_recv_cb(on_data_recv);
    if (ret != ESP_OK) {
        ESP_LOGE(_log_tag, "Falha ao registrar callback de recebimento: %s", esp_err_to_name(ret));
        return ret;
    }

    // 3. Adiciona a Base como Peer
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, MAC_ADDRESS_GROUND, 6); // Use MAC from header
    peerInfo.channel = 0;
    peerInfo.ifidx = WIFI_IF_STA;
    peerInfo.encrypt = true; // Encrypt packets to ground station
    memcpy(peerInfo.lmk, ROCKET_LMK, 16);
    
    ret = esp_now_add_peer(&peerInfo);
    if (ret != ESP_OK) {
        ESP_LOGE(_log_tag, "Falha ao adicionar peer: %s", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(_log_tag, "ESP-NOW Iniciado com Criptografia (PMK Set)");
    return ESP_OK;
}

void send_telemetry_packet(const void* data, size_t len) {
    esp_now_send(MAC_ADDRESS_GROUND, (uint8_t*)data, len);
}