#include "flight_tasks.h"
#include "globals.h"
#include "esp_task_wdt.h"
#include "esp_log.h"
#include "espnow_setup.h"
#include "sensors.h"
#include "storage.h"

// Variáveis de controle locais
static volatile float target_pitch = 0.0;
static volatile int target_throttle = 0;

// --- TAREFA 1: CONTROLE DE VOO (Alta Prioridade - Core 1) ---
void flight_task(void *pvParameters) {
    // Inicializa Watchdog (3 segundos)
    esp_task_wdt_init(3, true);
    esp_task_wdt_add(NULL);

    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(2); // 500Hz Loop

    while (1) {
        // Variável temporária para dados do loop atual
        TelemetryPacket sensorData;

        // 1. Ler Sensores (IMU, Barômetro)
        sensors_read(&sensorData);

        // 2. Máquina de Estados
        switch (currentState) {
            case STATE_ABORT:
                target_throttle = 0;
                break;
            // ...
        }

        // 3. PID Loop
        // ...

        // 4. Resetar Watchdog
        esp_task_wdt_reset();
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }

// --- TAREFA 2: COMUNICAÇÃO (Baixa Prioridade - Core 0) ---
void comms_task(void *pvParameters) {
    // MELHORIA: Adicionando Watchdog também na tarefa de comunicação
    // Se o rádio travar, o sistema reinicia
    esp_task_wdt_add(NULL);

    UplinkPacket packet;
    TelemetryPacket telemetry;

    while (1) {
        // 1. Processar Comandos Recebidos
        if (xQueueReceive(uplink_queue, &packet, 0)) {
            if (packet.magic_byte == 0xCAFEBABE) {
                if (packet.type == CMD_CHANGE_STATE) {
                    // Lógica de segurança
                    if (packet.data.target_state == STATE_ABORT) {
                        currentState = STATE_ABORT;
                        ESP_LOGE(TAG_FLIGHT, "!!! ABORTAR !!!");
                    }
                    else if (currentState == STATE_IDLE && packet.data.target_state == STATE_ARMED) {
                        currentState = STATE_ARMED;
                        ESP_LOGW(TAG_FLIGHT, "SISTEMA ARMADO");
                    }
                }
            }
        }

        // 2. Enviar Telemetria (10Hz)
        telemetry.current_state = currentState;
        telemetry.timestamp = xTaskGetTickCount();
        
        send_telemetry_packet(&telemetry, sizeof(telemetry));
        
        // MELHORIA: Blackbox - Grava dados na flash
        storage_log(&telemetry);

        // Resetar Watchdog da Comms Task
        esp_task_wdt_reset();

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}