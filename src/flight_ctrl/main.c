#include "nvs_flash.h"
#include "esp_log.h"

// Módulos organizados
#include "globals.h"
#include "wifi_setup.h"
#include "espnow_setup.h"
#include "ota_setup.h"
#include "flight_tasks.h"
#include "sensors.h"
#include "storage.h"

// --- VARIÁVEIS GLOBAIS (Voláteis para acesso entre tarefas) ---
volatile FlightState currentState = STATE_INIT;
QueueHandle_t uplink_queue;

// --- MAIN (Setup) ---
void app_main() {
    // 1. Inicialização de Sistema
    nvs_flash_init();
    
    // 2. Inicialização de Módulos
    wifi_init();
    ota_init();
    sensors_init();
    storage_init();
    
    // 3. Configuração de Comunicação
    uplink_queue = xQueueCreate(10, sizeof(UplinkPacket));
    espnow_init_custom(uplink_queue);

    ESP_LOGI(TAG_FLIGHT, "Sistema Iniciado. Criando Tarefas...");

    // Core 0: Comunicação e WiFi
    xTaskCreatePinnedToCore(comms_task, "Comms", 4096, NULL, 1, NULL, 0);

    // Core 1: Controle Crítico (PID)
    xTaskCreatePinnedToCore(flight_task, "Flight", 4096, NULL, 10, NULL, 1);
}