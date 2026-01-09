#pragma once

#include <stdint.h>

// --- ATENÇÃO: Substitua estes valores pelos seus ---

// Endereço MAC da sua Estação Base (Ground Station)
static const uint8_t MAC_ADDRESS_GROUND[6] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

// Chave Primária (PMK) para criptografia ESP-NOW (16 caracteres)
static const char* ROCKET_PMK = "MinhaChavePMK123";

// Chave Local (LMK) para criptografia ESP-NOW (16 caracteres)
static const char* ROCKET_LMK = "MinhaChaveLMK456";