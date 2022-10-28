#ifndef AIRBALL_TELEMETRY_X11_SCREEN_H
#define AIRBALL_TELEMETRY_X11_SCREEN_H

#include "screen.h"

namespace airball {

class X11Screen : public Screen {
public:
  X11Screen(const int x, const int y);

  ~X11Screen() override;

  virtual void flush() override {}
};


}  // namespace airball

#endif //AIRBALL_TELEMETRY_X11_SCREEN_H
