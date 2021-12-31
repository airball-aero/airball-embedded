#ifndef AIRBALL_CONTROLLER_H
#define AIRBALL_CONTROLLER_H

#include <string>

#include "screen.h"
#include "user_input_source.h"
#include "../telemetry/telemetry_client.h"

namespace airball {

/**
 * A Controller manages the coordination between the various portions of the
 * Airball application, and contains all the platform independent parts of the
 * logic.
 */
class Controller {
public:
  /**
   * Create a new Controller.
   *
   * @param screen a target onto which to display the output.
   * @param settings_path path for settings file.
   * @param audio_device name of ALSA audio device for sound output.
   * @param telemetry a source of sensor data.
   * @param logger a place to log sensor (and other) data.
   */
  Controller(Screen* screen,
             const std::string& settings_path,
             const std::string& audio_device,
             TelemetryClient* telemetry);

  /**
   * Run the Controller. Generally, this function never returns.
   */
   void run();

private:
  Screen* screen_;
  const std::string settings_path_;
  const std::string audio_device_;
  TelemetryClient* telemetry_;
};

}  // namespace airball

#endif //AIRBALL_CONTROLLER_H
