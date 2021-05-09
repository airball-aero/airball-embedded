#ifndef METRIC_H
#define METRIC_H

class Metric {
 public:
  Metric(const char* name, const long interval)
    : name_(name), interval_(interval), n_(0L), min_(0.0f), max_(0.0f), tot_(0.0f), lat_(0.0f) {}
  ~Metric() {}

  void add(float x) {
    lat_ = x;
    tot_ += x;
    if (n_ == 0L) { min_ = max_ = x; }
    if (x < min_) { min_ = x; }
    if (x > max_) { max_ = x; }
    n_++;
  }

  void mark() { mark_ = micros(); }

  void record() {
    if (mark_ == 0L) { return; }
    add(micros() - mark_);
    mark_ = 0L;
  }
  
  bool ready() { return n_ >= interval_; }
  
  const char* str() {
    sprintf(buf_,
	    "name,%s,n,%ld,min,%f,max,%f,avg,%f,lat,%f",
	    name_,
	    n_,
	    min_,
	    max_,
	    tot_ / ((float) n_),
	    lat_);
    n_ = 0L;
    min_ = 0.0f;
    max_ = 0.0f;
    tot_ = 0.0f;
    lat_ = 0.0f;
    return buf_;
  }
  
 private:
  char buf_[1024];
  const char* name_;
  const long interval_;
  long mark_;
  long n_;
  float tot_;
  float min_;
  float max_;
  float lat_;
};

#endif
