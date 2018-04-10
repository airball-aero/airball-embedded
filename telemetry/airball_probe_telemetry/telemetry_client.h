#ifndef AIRBALL_TELEMETRY_CLIENT_H
#define AIRBALL_TELEMETRY_CLIENT_H

namespace airball {

/**
 * A client to a standard Airball telemetry server. A telemetry server
 * runs on a Linux machine and receives telemetry from some source.
 */
class TelemetryClient {
public:
  /**
   * A sample of airdata acquired by the Airball probe.
   */
  struct Airdata {
    /** The barometric pressure (Pa) */
    double baro;
    
    /** The outside air temperature (degrees C) */
    double oat;

    /** The center hole gage pressure (Pa) */
    double dp0;

    /** The AoA axis pressure hole difference (Pa) */
    double dpA;

    /** The yaw axis pressure hole difference (Pa) */
    double dpB;
  };

  /**
   * A message describing the technical status of the Airball probe.
   */
  struct ProbeStatus {
    /** The battery voltage (V) */
    double voltage;

    /** The battery current (A) */
    double current;

    /** The battery capacity (Ah) */
    double capacity;

    /** The battery capacity as a ratio of full scale [0..1] */
    double capacity_ratio;
  };

  /**
   * A message describing the technical status of the wireless link to
   * the Airball probe.
   */
  struct LinkStatus {
    /**
     * The Received Signal Strength Indicator (RSSI) as a proportion of
     * the manufacturer specified maximum [0..1]
     */
    double rssi_ratio;
  };

  enum DatumType {
    AIRDATA,
    PROBE_STATUS,
    LINK_STATUS,
  };

  struct Datum {
    DatumType type;
    union {
      Airdata airdata;
      ProbeStatus probe_status;
      LinkStatus link_status;
    };
  };

  /**
   * Blocks until a Datum is available, then returns it.
   */
  virtual Datum get() = 0;
};

}  // namespace airball

#endif //AIRBALL_TELEMETRY_CLIENT_H
