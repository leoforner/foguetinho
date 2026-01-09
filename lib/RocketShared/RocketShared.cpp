#include "RocketShared.h"
#include <string.h>

const char* getFlightStateName(FlightState state) {
    switch(state) {
        case STATE_INIT: return "INIT";
        case STATE_IDLE: return "IDLE";
        case STATE_ARMED: return "ARMED";
        case STATE_ASCENT: return "ASCENT";
        case STATE_HOVER: return "HOVER";
        case STATE_DESCENT: return "DESCENT";
        case STATE_LANDING: return "LANDING";
        case STATE_LANDED: return "LANDED";
        case STATE_ABORT: return "ABORT";
        default: return "UNKNOWN";
    }
}

void initUplinkPacket(UplinkPacket* pkt, UplinkType type) {
    memset(pkt, 0, sizeof(UplinkPacket));
    pkt->type = type;
    pkt->magic_byte = 0xCAFEBABE;
}