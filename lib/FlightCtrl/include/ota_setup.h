#pragma once

// Estrutura para manter o contexto do módulo OTA
typedef struct {
    const char* log_tag;
    // Adicione outras dependências aqui, se necessário.
    // Ex: volatile FlightState* flight_state;
} ota_context_t;

// Inicializa a configuração OTA para o Flight Controller
void ota_init(const ota_context_t* context);

// Verifica se há atualizações de firmware
void ota_check_updates(void);