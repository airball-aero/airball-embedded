#ifndef AIRBALL_PROBE_TELEMETRY_SAMPLE_H
#define AIRBALL_PROBE_TELEMETRY_SAMPLE_H

#include <string>
#include <vector>
#include <map>
#include <ctime>
#include <chrono>

class sample {
private:

    static const char *TIME_FORMAT;

    std::chrono::time_point<std::chrono::system_clock> time;
    uint8_t rssi;

public:

    sample() {};

    sample(std::chrono::time_point<std::chrono::system_clock> time, uint8_t rssi)
            : time(time), rssi(rssi) {
    };

    virtual std::string type() {
        return "unknown";
    }

    static std::vector<std::string> split(std::string in) {
        std::vector<std::string> out;

        size_t last = 0;
        size_t next = 0;
        while((next = in.find(',', last)) != std::string::npos) {
            out.emplace_back(in.substr(last, next-last).data());
            last = next + 1;
        }
        out.emplace_back(in.substr(last).data());

        return out;
    }

    std::string format_time() {
        char time_buf[200];
        double unix_time = std::chrono::duration_cast<std::chrono::milliseconds>(time.time_since_epoch()).count() / 1000.0;
        sprintf(time_buf, "%.3f", unix_time);

        return std::string(time_buf);
    }

    std::chrono::time_point<std::chrono::system_clock> get_time() const { return time; }
    uint8_t get_rssi() const { return rssi; }

    virtual int snprintf(const char *str, size_t len) {
        return std::snprintf((char *)str, len, "%s", format_time().c_str());
    };

    virtual std::string format() {
        std::string result;
        int len = snprintf(nullptr, (size_t) 0);
        if (len > 0) {
            result.reserve((unsigned long) len+1);
            snprintf(result.c_str(), (size_t) len+1);
            return result;
        }
        return "";
    }
};

#endif // AIRBALL_PROBE_TELEMETRY_SAMPLE_H
