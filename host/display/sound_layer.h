#ifndef AIRBALL_DISPLAY_SOUND_LAYER_H
#define AIRBALL_DISPLAY_SOUND_LAYER_H

#include <cstddef>

namespace airball {

class sound_layer {
public:
  explicit sound_layer(size_t period);
  virtual ~sound_layer() = default;

  size_t period() const;

  virtual void apply(float* buf, size_t frames, size_t pos) const = 0;

private:
  const size_t period_;
};

} // namespace airball

#endif // AIRBALL_DISPLAY_SOUND_LAYER_H
