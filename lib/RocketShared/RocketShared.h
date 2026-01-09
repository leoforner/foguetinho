#ifndef ROCKET_SHARED_H
#define ROCKET_SHARED_H

#include <stdint.h>

// --- ESTADOS DE VOO (FSM) ---
typedef enum {
    STATE_INIT = 0,
    STATE_IDLE,      // 1: Esperando na rampa
    STATE_ARMED,     // 2: Pronto para lançamento
    STATE_ASCENT,    // 3: Subindo
    STATE_HOVER,     // 4: Estabilização
    STATE_DESCENT,   // 5: Descendo
    STATE_LANDING,   // 6: Pouso final
    STATE_LANDED,    // 7: No chão
    STATE_ABORT      // 8: EMERGÊNCIA
} FlightState;

// --- PACOTE DE TELEMETRIA (Foguete -> Base) ---
typedef struct {
    uint32_t timestamp;
    FlightState current_state;
    float altitude;
    float velocity_z;
    float accel_z;
    float angle_pitch;
    float bat_voltage;
} __attribute__((packed)) TelemetryPacket;

// --- PACOTE DE COMANDO (Base -> Foguete) ---
enum UplinkType {
    CMD_PING = 0,
    CMD_CHANGE_STATE,
    CMD_MANUAL_CTRL
};

typedef struct {
    uint8_t type;
    uint32_t magic_byte; // 0xCAFEBABE
    
    union {
        FlightState target_state;
        struct {
            int16_t throttle;
            int16_t pitch;
            int16_t yaw;
        } manual;
    } data;
} __attribute__((packed)) UplinkPacket;

// --- ASSINATURAS DAS FUNÇÕES (Implementadas no .cpp) ---
// Retorna o nome do estado em texto (ex: "ASCENT") para facilitar o Debug
const char* getFlightStateName(FlightState state);

// Cria um pacote de comando padrão com o Magic Byte já preenchido
void initUplinkPacket(UplinkPacket* pkt, UplinkType type);

// --- SEGURANÇA (ESP-NOW Encryption) ---
// Devem ter exatamente 16 caracteres
#define ROCKET_PMK "FoguetePMKKey123" // Primary Master Key
#define ROCKET_LMK "FogueteLMKKey123" // Local Master Key

#endif