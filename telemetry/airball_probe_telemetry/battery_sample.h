#ifndef AIRBALL_PROBE_TELEMETRY_BATTERY_SAMPLE_H
#define AIRBALL_PROBE_TELEMETRY_BATTERY_SAMPLE_H

#include "sample.h"

class battery_sample : public sample {
private:

    unsigned long seq;
    double voltage;
    double current;
    double capacity_mAh;
    double capacity_pct;

    enum FieldPositions {
        FIELD_PREFIX = 0,
        FIELD_SEQ = 1,
        FIELD_VOLTAGE = 2,
        FIELD_CURRENT = 3,
        FIELD_CAPACITY_MAH = 4,
        FIELD_CAPACITY_PCT = 5,
        FIELD_COUNT = 6,
    };

public:

    unsigned long get_seq() const { return seq; }
    double get_voltage() const { return voltage; }
    double get_current() const { return current; }
    double get_capacity_mAh() const { return capacity_mAh; }
    double get_capacty_pct() const { return capacity_pct; }

    static const char *PREFIX;

    static sample *create(std::chrono::time_point<std::chrono::system_clock> time, uint8_t rssi, std::vector<std::string> fields) {
        if (fields.at(FIELD_PREFIX) == PREFIX && fields.size() == FIELD_COUNT)
            return new battery_sample(time, rssi,
                                      atoi(fields.at(FIELD_SEQ).c_str()),
                               atof(fields.at(FIELD_VOLTAGE).c_str()),
                               atof(fields.at(FIELD_CURRENT).c_str()),
                               atof(fields.at(FIELD_CAPACITY_MAH).c_str()),
                               atof(fields.at(FIELD_CAPACITY_PCT).c_str()));

        return nullptr;
    }

    battery_sample(std::chrono::time_point<std::chrono::system_clock> time, uint8_t rssi, unsigned long seq, double voltage, double current, double capacity_mAh, double capacity_pct)
    :sample(time, rssi), seq(seq), voltage(voltage), current(current), capacity_mAh(capacity_mAh), capacity_pct(capacity_pct) {

    };

    std::string type() {
        return "battery";
    }

    int snprintf(const char *str, size_t len) {
        return std::snprintf((char *)str, len, "%s,%d,%ld,%.2f,%.2f,%.2f,%.2f",
                             format_time().c_str(), get_rssi(), seq, voltage, current, capacity_mAh, capacity_pct);
    }

};


#endif // AIRBALL_PROBE_TELEMETRY_BATTERY_SAMPLE_H
