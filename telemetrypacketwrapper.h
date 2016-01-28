#ifndef TELEMETRYPACKETWRAPPER
#define TELEMETRYPACKETWRAPPER

#include "proto/telemetry.pb.h"

class TelemetryPacketWrapper {
public:
    static double packet_id(const TelemetryPacket& p) {
        return p.packet_id();
    }
    static double temperature_internal(const TelemetryPacket& p) {
        return p.temperature_internal();
    }
    static double temperature_external(const TelemetryPacket& p) {
        return p.temperature_external();
    }
    static double altitude(const TelemetryPacket& p) {
        return p.altitude();
    }
    static double latitude(const TelemetryPacket& p) {
        return p.latitude();
    }
    static double longitude(const TelemetryPacket& p) {
        return p.longitude();
    }
    static double pressure(const TelemetryPacket& p) {
        return p.pressure();
    }
    static double bmp180_temperature(const TelemetryPacket& p) {
        return p.bmp180_temperature();
    }
    static double voltage(const TelemetryPacket& p) {
        return p.voltage();
    }
    static double radiation(const TelemetryPacket& p) {
        return p.radiation();
    }
};

#endif // TELEMETRYPACKETWRAPPER

