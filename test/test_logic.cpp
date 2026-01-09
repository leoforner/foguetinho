#if defined(ARDUINO)
#include <Arduino.h>
#elif defined(ESP_PLATFORM)
// Includes específicos do ESP-IDF
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#endif

#include <unity.h>
#include <string.h> // Necessário para memset/strcmp no ambiente nativo
#include "RocketShared.h"

// --- TESTES UNITÁRIOS ---

// Teste 1: Verifica se o pacote de comando é inicializado com segurança
void test_uplink_packet_security(void)
{
    UplinkPacket pkt;

    // Preenche com lixo de memória para garantir que a função limpa tudo
    memset(&pkt, 0xFF, sizeof(UplinkPacket));

    // Chama a função do seu código
    initUplinkPacket(&pkt, CMD_MANUAL_CTRL);

    // VERIFICAÇÕES:
    // 1. O Magic Byte deve ser 0xCAFEBABE
    TEST_ASSERT_EQUAL_HEX32(0xCAFEBABE, pkt.magic_byte);

    // 2. O tipo deve ser o que pedimos
    TEST_ASSERT_EQUAL_UINT8(CMD_MANUAL_CTRL, pkt.type);
}

// Função auxiliar para SIMULAR a lógica de apogeu (Pure Function)
// Em produção, lógica similar estaria no flight_tasks.c, mas aqui isolamos para testar
FlightState simulate_apogee_logic(FlightState current_state, float velocity_z)
{
    // Lógica: Se estamos subindo (ASCENT) e a velocidade vertical fica negativa
    if (current_state == STATE_ASCENT)
    {
        // Threshold de -0.5 m/s para evitar ruído do sensor
        if (velocity_z < -0.5f)
        {
            return STATE_DESCENT;
        }
    }
    return current_state;
}

// Teste 3: Simulação de Detecção de Apogeu
void test_apogee_detection(void)
{
    // 1. Foguete subindo rápido (10 m/s) -> Deve manter ASCENT
    TEST_ASSERT_EQUAL(STATE_ASCENT, simulate_apogee_logic(STATE_ASCENT, 10.0f));

    // 2. Foguete parando no topo (0 m/s) -> Deve manter ASCENT (ainda não caiu)
    TEST_ASSERT_EQUAL(STATE_ASCENT, simulate_apogee_logic(STATE_ASCENT, 0.0f));

    // 3. Foguete começando a cair (-1.0 m/s) -> Deve mudar para DESCENT
    TEST_ASSERT_EQUAL(STATE_DESCENT, simulate_apogee_logic(STATE_ASCENT, -1.0f));

    // 4. Garantir que não muda se já estiver em outro estado (ex: IDLE)
    TEST_ASSERT_EQUAL(STATE_IDLE, simulate_apogee_logic(STATE_IDLE, -1.0f));
}

// Teste 2: Verifica se a conversão de Estado para Texto funciona (para Debug)
void test_state_naming(void)
{
    TEST_ASSERT_EQUAL_STRING("ASCENT", getFlightStateName(STATE_ASCENT));
    TEST_ASSERT_EQUAL_STRING("ABORT", getFlightStateName(STATE_ABORT));
}

void run_all_tests()
{
    UNITY_BEGIN();
    RUN_TEST(test_uplink_packet_security);
    RUN_TEST(test_state_naming);
    RUN_TEST(test_apogee_detection);
    UNITY_END();
}

#if defined(ARDUINO)
void setup()
{
    delay(2000); // Espera conexão serial
    run_all_tests();
}

void loop() {}
#elif defined(ESP_PLATFORM)
// Entry point para ESP-IDF (Ambiente flight_ctrl)
extern "C" void app_main()
{
    vTaskDelay(2000 / portTICK_PERIOD_MS); // Espera monitor serial
    run_all_tests();
}
#else // NATIVE (PC)
int main(int argc, char **argv)
{
    run_all_tests();
    return 0;
}
#endif