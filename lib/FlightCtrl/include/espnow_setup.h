#pragma once
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "esp_err.h" // Adicionar para esp_err_t

// Estrutura de contexto para o módulo ESP-NOW
typedef struct {
    QueueHandle_t uplink_queue; // Fila para onde os comandos recebidos serão enviados
    const char* log_tag;        // Tag para logs (opcional)
} espnow_context_t;

// Inicializa ESP-NOW usando o contexto fornecido
esp_err_t espnow_init_custom(const espnow_context_t* context);

// Envia telemetria (wrapper)
void send_telemetry_packet(const void* data, size_t len);