#ifndef AIRBALL_PROBE_TELEMETRY_SAMPLER_H
#define AIRBALL_PROBE_TELEMETRY_SAMPLER_H


#include "sample.h"
#include "airdata_sample.h"
#include "battery_sample.h"

class sampler {
private:
    typedef sample *(creator_t)(std::chrono::time_point<std::chrono::system_clock>, uint8_t, std::vector<std::string>) ;
    std::map<std::string, creator_t *> creators;

public:

    sampler() {};

    void add_sample_type(std::string prefix, creator_t creator) {
        creators[prefix] = creator;
    }

    std::vector<std::string> types() {
        std::vector<std::string> types;
        for (auto creator : creators) {
            types.push_back(creator.first);
        }
        return types;
    }

    sample *parse(std::chrono::time_point<std::chrono::system_clock> time, uint8_t rssi, std::string line) {
        auto fields = sample::split(line);

        std::string prefix = fields.at(0);

        creator_t *create = creators[prefix];

        if (create)
            return create(time, rssi, fields);

        return nullptr;
    }

};

#endif // AIRBALL_PROBE_TELEMETRY_SAMPLER_H
