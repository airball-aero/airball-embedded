#ifndef AIRBALL_PROBE_TELEMETRY_AIRDATA_SAMPLE_H
#define AIRBALL_PROBE_TELEMETRY_AIRDATA_SAMPLE_H

#include "sample.h"

class airdata_sample : public sample {
private:

    unsigned long seq;
    double baro;
    double temperature;
    double dp0;
    double dpA;
    double dpB;

    enum FieldPositions {
        FIELD_PREFIX = 0,
        FIELD_SEQ = 1,
        FIELD_BARO = 2,
        FIELD_TEMPERATURE = 3,
        FIELD_DP0 = 4,
        FIELD_DPA = 5,
        FIELD_DPB = 6,
        FIELD_COUNT = 7,
    };

public:

    unsigned long get_seq() const { return seq; }
    double get_baro() const { return baro; }
    double get_temperature() const { return temperature; }
    double get_dp0() const { return dp0; }
    double get_dpA() const { return dpA; }
    double get_dpB() const { return dpB; }

    static const char *PREFIX;

    static sample *create(std::chrono::time_point<std::chrono::system_clock> time, uint8_t rssi, std::vector<std::string> fields) {
        if (fields.at(FIELD_PREFIX) == PREFIX && fields.size() == FIELD_COUNT)
            return new airdata_sample(time, rssi,
                                      atoi(fields.at(FIELD_SEQ).c_str()),
                                       atof(fields.at(FIELD_BARO).c_str()),
                                       atof(fields.at(FIELD_TEMPERATURE).c_str()),
                                       atof(fields.at(FIELD_DP0).c_str()),
                                       atof(fields.at(FIELD_DPA).c_str()),
                                       atof(fields.at(FIELD_DPB).c_str()));

        return nullptr;
    }

    airdata_sample(std::chrono::time_point<std::chrono::system_clock> time, uint8_t rssi, unsigned long seq, double baro, double temperature, double dp0, double dpA, double dpB)
            :sample(time, rssi), seq(seq), baro(baro), temperature(temperature), dp0(dp0), dpA(dpA), dpB(dpB) {

    };

    std::string type() {
        return "airdata";
    }

    virtual int snprintf(const char *str, size_t len) const {
        return std::snprintf((char *)str, len, "%s,%d,%ld,%.2f,%.2f,%.2f,%.2f,%.2f",
                             format_time().c_str(), get_rssi(), seq, baro, temperature, dp0, dpA, dpB);
    }

};

#endif // AIRBALL_PROBE_TELEMETRY_AIRDATA_SAMPLE_H
