#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "RocketShared.h"

// Variáveis globais definidas no main.c
extern volatile FlightState currentState;
extern QueueHandle_t uplink_queue;

// Constante para TAG de Log
#define TAG_FLIGHT "FLIGHT_CTRL"