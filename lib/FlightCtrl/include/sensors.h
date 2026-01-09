#pragma once
#include "RocketShared.h"

// Configura I2C e inicializa os sensores (MPU, Barômetro)
void sensors_init(void);

// Lê os dados brutos e aplica fusão de sensores
// Preenche a struct de telemetria com os dados processados
void sensors_read(TelemetryPacket* data);

// Calibra o giroscópio (deve ser chamado com o foguete parado)
void sensors_calibrate(void);