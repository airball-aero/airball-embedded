#include "file_write_watch.h"

#include <sys/inotify.h>
#include <unistd.h>

airball::file_write_watch::file_write_watch(const std::string& path) {
  fd_ = inotify_init();
  wd_ = inotify_add_watch(fd_, path.c_str(), IN_CLOSE_WRITE);
}

airball::file_write_watch::~file_write_watch() {
  inotify_rm_watch(fd_, wd_);
}

void
airball::file_write_watch::next_event() {
  while (true) {
    inotify_event e;
    read(fd_, &e, sizeof(e));
    if (e.wd == wd_) {
      return;
    }
  }
}
