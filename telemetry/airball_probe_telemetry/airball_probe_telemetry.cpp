#include <iostream>

#include <fstream>

#include "sample.h"
#include "sampler.h"
#include "xbee.h"
#include "format.h"

int main(int argc, char **argv) {
    const std::string airball_serial_device_filename = std::string(argv[1]);

    printf("%s Opening serial port at %s...\n",
           airball::format_time(std::chrono::system_clock::now()).c_str(),
           airball_serial_device_filename.c_str());

    xbee radio(airball_serial_device_filename, 9600);

    radio.enterCommandMode();

    radio.sendCommand("ATNIAIRBALL_BASE");
    radio.sendCommand("ATID=5555");
    radio.sendCommand("ATMY=8888");
    radio.sendCommand("ATAP=1");

    radio.exitCommandMode();

    sampler telemetry;

    telemetry.add_sample_type(airdata_sample::PREFIX, airdata_sample::create);
    telemetry.add_sample_type(battery_sample::PREFIX, battery_sample::create);

    // This is really dumb.
    std::vector<std::string> file_types;
    file_types.emplace_back("airdata");
    file_types.emplace_back("battery");

    std::map<std::string, std::ofstream *> files;
    std::map<std::string, int> stats;
    stats["unusable"] = 0;

    auto log_time = std::chrono::system_clock::now();

    for (const std::string& type : file_types) {
        std::ofstream *file = new std::ofstream;
        std::string filename = airball::format_log_filename(log_time, type);
        file->open(filename, std::ios::out);
        files[type] = file;
        stats[type] = 0;

        printf("%s Logging %s data to %s\n",
               airball::format_time(std::chrono::system_clock::now()).c_str(),
               type.c_str(), filename.c_str());
    }

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    for (int packet_number=0; ; packet_number++) {
        xbee_packet packet = radio.read_packet();

        auto current_time = std::chrono::system_clock::now();

        if (packet.frame_type == xbee_packet::PacketTypeReceive16Bit) {
            xbee_packet::PacketReceive16Bit data(packet.frame_data);

            if (sample *s = telemetry.parse(current_time, data.rssi(), data.data())) {
                stats[s->type()]++;
                *files[s->type()] << s->format().c_str() << std::endl;
            } else {
                stats["unusable"]++;
                printf("%s [%10d] Unusable: %s\n",
                       airball::format_time(current_time).c_str(),
                       packet_number, data.data().c_str());
            }

            if (packet_number % 100 == 0) {
                printf("%s [%10d] Stats:",
                       airball::format_time(current_time).c_str(),
                       packet_number);
                for (auto stat : stats) {
                    printf(" %s=%d", stat.first.c_str(), stat.second);
                }
                printf("\n");
            }
        }
    }
#pragma clang diagnostic pop
    return 0;
}