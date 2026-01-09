#include <unity.h>
#include "espnow_setup.h"
#include "RocketShared.h"

// Apenas compila este arquivo se estivermos no ESP32 (ignora no PC)
#ifdef ESP_PLATFORM

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <esp_log.h>

// Teste: Inicialização do ESP-NOW
void test_espnow_init_success(void)
{
    // 1. Prepara o contexto necessário (Fila)
    espnow_context_t ctx;
    ctx.uplink_queue = xQueueCreate(5, sizeof(UplinkPacket));
    ctx.log_tag = "TEST_ESPNOW";

    TEST_ASSERT_NOT_NULL_MESSAGE(ctx.uplink_queue, "Falha ao criar fila de teste");

    // 2. Tenta inicializar
    // Nota: Isso requer que o WiFi esteja inicializado ou que a lib trate isso.
    // Se sua lib espnow_setup não init o WiFi, pode falhar aqui.
    // Assumindo que esp_now_init requer wifi, mas vamos testar a função wrapper.

    // Para este teste funcionar isolado, talvez precisemos init o NVS/WiFi antes,
    // mas vamos ver se sua função lida com isso ou retorna erro esperado.

    // Se falhar porque o WiFi não está ativo, o teste é válido: descobrimos uma dependência!
    esp_err_t ret = espnow_init_custom(&ctx);

    // Verifica se retornou ESP_OK (0)
    TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, ret, "ESP-NOW falhou ao iniciar");

    // Limpeza
    vQueueDelete(ctx.uplink_queue);
}

extern "C" void app_main()
{
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    UNITY_BEGIN();
    RUN_TEST(test_espnow_init_success);
    UNITY_END();
}

#endif