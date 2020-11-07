#ifndef AIRBALL_PROBE_TELEMETRY_AIRDATA_REDUCED_SAMPLE_H
#define AIRBALL_PROBE_TELEMETRY_AIRDATA_REDUCED_SAMPLE_H

#include "sample.h"

class airdata_reduced_sample : public sample {
private:

  unsigned long seq;
  double alpha;
  double beta;
  double q;
  double baro;
  double temperature;

  enum FieldPositions {
    FIELD_PREFIX = 0,
    FIELD_SEQ = 1,
    FIELD_ALPHA = 2,
    FIELD_BETA = 3,
    FIELD_Q = 4,
    FIELD_BARO = 5,
    FIELD_TEMPERATURE = 6,
    FIELD_COUNT = 7,
  };

public:

  unsigned long get_seq() const { return seq; }

  double get_alpha() const { return alpha; }

  double get_beta() const { return beta; }

  double get_q() const { return q; }

  double get_baro() const { return baro; }

  double get_temperature() const { return temperature; }

  static const char *PREFIX;

  static sample *
  create(std::chrono::time_point<std::chrono::system_clock> time, uint8_t rssi,
         std::vector<std::string> fields) {
    if (fields.at(FIELD_PREFIX) == PREFIX && fields.size() == FIELD_COUNT)
      return new airdata_reduced_sample(time, rssi,
                                        atoi(fields.at(FIELD_SEQ).c_str()),
                                        atof(fields.at(FIELD_ALPHA).c_str()),
                                        atof(fields.at(FIELD_BETA).c_str()),
                                        atof(fields.at(FIELD_Q).c_str()),
                                        atof(fields.at(FIELD_BARO).c_str()),
                                        atof(fields.at(
                                            FIELD_TEMPERATURE).c_str()));

    return nullptr;
  }

  airdata_reduced_sample(
      std::chrono::time_point<std::chrono::system_clock> time, uint8_t rssi,
      unsigned long seq, double alpha, double beta, double q, double baro,
      double temperature)
      : sample(time, rssi), seq(seq), alpha(alpha), beta(beta), q(q),
        baro(baro), temperature(temperature) {
  };

  std::string type() {
    return "airdata_reduced";
  }

  virtual int snprintf(const char *str, size_t len) const {
    return std::snprintf((char *) str, len,
                         "%s,%d,%ld,%.2f,%.2f,%.2f,%.2f,%.2f",
                         format_time().c_str(), get_rssi(), seq, alpha, beta, q,
                         baro, temperature);
  }
};

#endif // AIRBALL_PROBE_TELEMETRY_AIRDATA_REDUCED_SAMPLE_H
