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
   * The common base class of all Airball telemetry.
   */
  class Datum {};

  /**
   * A sample of airdata acquired by the Airball probe.
   */
  class Airdata : public Datum {
  public:
    Airdata(double baro, double oat, double dp0, double dpA, double dpB)
      : baro(baro), oat(oat), dp0(dp0), dpA(dpA), dpB(dpB) {}
    
    /** The barometric pressure (Pa) */
    const double baro;
    
    /** The outside air temperature (degrees C) */
    const double oat;

    /** The center hole gage pressure (Pa) */
    const double dp0;

    /** The AoA axis pressure hole difference (Pa) */
    const double dpA;

    /** The yaw axis pressure hole difference (Pa) */
    const double dpB;
  };

  /**
   * A message describing the technical status of the Airball probe.
   */
  class ProbeStatus : public Datum {
  public:
    ProbeStatus(double voltage, double current, double capacity, double capacity_pct)
      : voltage(voltage), current(current), capacity(capacity), capacity_pct(capacity_pct) {}

    /** The battery voltage (V) */
    const double voltage;

    /** The battery current (A) */
    const double current;

    /** The battery capacity (Ah) */
    const double capacity;

    /** The battery capacity as a ratio of full scale [0..1] */
    const double capacity_ratio;
  };

  /**
   * A message describing the technical status of the wireless link to
   * the Airball probe.
   */
  class LinkStatus : public Datum {
  public:
    LinkStatus(double rssi)
      : rssi(rssi) {}

    /**
     * The Received Signal Strength Indicator (RSSI) as a proportion of
     * the manufacturer specified maximum [0..1]
     */
    const double rssi_ratio;
  }

  virtual ~TelemetryClient() {}

  /**
   * Blocks until a Datum is available, then returns it.
   */
  Datum get() = 0;
};

}  // namespace airball

#endif //AIRBALL_TELEMETRY_CLIENT_H
