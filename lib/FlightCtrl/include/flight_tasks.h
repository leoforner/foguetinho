#pragma once

// Tarefa de Controle de Voo (PID, Sensores) - Core 1
void flight_task(void *pvParameters);

// Tarefa de Comunicação (ESP-NOW, Telemetria) - Core 0
void comms_task(void *pvParameters);