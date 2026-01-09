#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include "RocketShared.h"
#include "mac_addresses.h" // centralized MAC addresses
#include <FastLED.h>
#include <Bounce2.h>

#define LED_PIN     2   // Pino do LED RGB (Ex: GPIO 2 ou outro)
#define NUM_LEDS    1
#define BUTTON_PIN  4   // Pino do Botão de Armar/Lançar

esp_now_peer_info_t peerInfo;
TelemetryPacket lastTelemetry;

CRGB leds[NUM_LEDS];
Bounce debouncer = Bounce();

// --- RECEPÇÃO (Foguete -> Base) ---
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
    if (len == sizeof(TelemetryPacket)) {
        memcpy(&lastTelemetry, incomingData, sizeof(TelemetryPacket));
        
        // Formata para o Serial Plotter ou Monitor
        Serial.print("Estado:"); Serial.print(lastTelemetry.current_state);
        Serial.print(" Alt:"); Serial.println(lastTelemetry.altitude);

        // Atualiza LED baseado no estado
        switch (lastTelemetry.current_state) {
            case STATE_IDLE:
                leds[0] = CRGB::Green;
                break;
            case STATE_ARMED:
                leds[0] = CRGB::Red;
                break;
            case STATE_ASCENT:
                leds[0] = CRGB::Blue;
                break;
            case STATE_ABORT:
                leds[0] = CRGB::Purple; // Emergência
                break;
            default:
                leds[0] = CRGB::Yellow;
        }
        FastLED.show();
    }
}

// --- ENVIO (Base -> Foguete) ---
void sendCommand(UplinkType type, FlightState stateOpt) {
    UplinkPacket pkt;
    
    // Inicializa limpando lixo de memória e pondo o Magic Byte
    initUplinkPacket(&pkt, type); 
    
    pkt.data.target_state = stateOpt;
    esp_err_t result = esp_now_send(MAC_ADDRESS_FLIGHT, (uint8_t *) &pkt, sizeof(pkt));
    if (result != ESP_OK) {
        Serial.println("Erro no envio!");
    }
}

// --- SETUP ---

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);

    // Configura FastLED
    FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
    
    // Configura Botão com Debounce
    debouncer.attach(BUTTON_PIN, INPUT_PULLUP);
    debouncer.interval(25);

    // Configura PMK (Primary Master Key) para criptografia
    if (esp_now_set_pmk((uint8_t *)ROCKET_PMK) != ESP_OK) {
        Serial.println("Erro ao definir PMK");
    }

    if (esp_now_init() != ESP_OK) {
        Serial.println("Erro ESP-NOW");
        return;
    }

    esp_now_register_recv_cb(OnDataRecv);

    // Registra o Foguete
    memcpy(peerInfo.peer_addr, MAC_ADDRESS_FLIGHT, 6);
    peerInfo.channel = 0;  
    peerInfo.encrypt = true; // Habilita Criptografia
    memcpy(peerInfo.lmk, ROCKET_LMK, 16); // Define a chave local (LMK)

    if (esp_now_add_peer(&peerInfo) != ESP_OK){
        Serial.println("Falha ao parear com foguete");
    }
}

void loop() {
    // Atualiza botão
    debouncer.update();
    if (debouncer.fell()) { // Botão pressionado
        Serial.println("Botão pressionado: Enviando comando ARMAR/LANÇAR");
        // Exemplo simples: Se estiver IDLE, manda ARMAR.
        sendCommand(CMD_CHANGE_STATE, STATE_ARMED);
    }

    // Lê comandos do Teclado/PC
    if (Serial.available()) {
        char cmd = Serial.read();
        
        if (cmd == 'A') {
            Serial.println("Enviando ARMAR...");
            sendCommand(CMD_CHANGE_STATE, STATE_ARMED);
        }
        else if (cmd == 'X') {
            Serial.println("!!! ABORTAR !!!");
            sendCommand(CMD_CHANGE_STATE, STATE_ABORT);
        }
    }
}