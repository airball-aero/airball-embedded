#include "system_status.h"

namespace airball {

void SystemStatus::update(TelemetryClient::Datum d) {}

bool SystemStatus::flight_data_up() const { return true; }

double SystemStatus::link_quality() const { return 1.0; }

double SystemStatus::battery_health() const { return 1.0; }

}  // namespace airball