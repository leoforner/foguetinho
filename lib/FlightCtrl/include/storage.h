#pragma once
#include "RocketShared.h"

// Inicializa o sistema de arquivos (SPIFFS)
void storage_init(void);

// Salva um pacote de telemetria no arquivo de log
// Retorna 0 em sucesso, negativo em erro
int storage_log(const TelemetryPacket* data);