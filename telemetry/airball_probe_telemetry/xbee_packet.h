#ifndef AIRBALL_PROBE_TELEMETRY_XBEE_PACKET_H
#define AIRBALL_PROBE_TELEMETRY_XBEE_PACKET_H


#include <vector>
#include <string>

class xbee_packet {
public:
    enum PacketTypes {
        PacketTypeATCommandRequest = 0x08,
        PacketTypeATCommandResponse = 0x88,
        PacketTypeATQueueParameterRequest = 0x09,
        PacketTypeRemoteATCommandRequest = 0x17,
        PacketTypeRemoteATCommandResponse = 0x97,
        PacketTypeTransmit64Bit = 0x00,
        PacketTypeTransmit16Bit = 0x01,
        PacketTypeReceive64Bit = 0x80,
        PacketTypeReceive16Bit = 0x81,
        PacketTypeTransmitStatus = 0x89,
        PacketTypeModemStatus = 0x8a,
    };

    class PacketReceive16Bit {
    private:
        std::vector<char> frame_data;
    public:
        explicit PacketReceive16Bit(std::vector<char>& frame_data) : frame_data(frame_data) {}

        uint16_t source_address() {
            return ((uint16_t)frame_data[0] << 8) | (uint16_t)frame_data[1];
        }

        uint8_t rssi() {
            return (uint8_t)frame_data[2];
        }

        uint8_t options() {
            return (uint8_t)frame_data[3];
        }

        std::string data() {
            return std::string {frame_data.data()+4, frame_data.size()-4};
        }
    };

    uint16_t length;
    uint8_t frame_type;
    std::vector<char> frame_data;
    uint8_t checksum;
};


#endif // AIRBALL_PROBE_TELEMETRY_XBEE_PACKET_H
