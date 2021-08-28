#ifndef AIRBALL_FILE_WRITE_WATCH_H
#define AIRBALL_FILE_WRITE_WATCH_H

#include <string>

namespace airball {

class file_write_watch {
public:
  file_write_watch(const std::string& path);
  ~file_write_watch();

  void next_event();

private:
  int fd_;
  int wd_;
};

} // namespace airball

#endif // AIRBALL_FILE_WRITE_WATCH_H
