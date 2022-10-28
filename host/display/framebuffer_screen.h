#include "screen.h"

namespace airball {

class FramebufferScreen : public Screen {
public:
  FramebufferScreen();

  ~FramebufferScreen() override;

  virtual void flush() override {}

private:
  void setUpFb();
  void tearDownFb();

  int fbfd_;
  unsigned char *fbp_;
  int xres_;
  int yres_;
  long int screensize_;
  cairo_surface_t *cs_;
  cairo_t *cr_;
};

}  // namespace airball
