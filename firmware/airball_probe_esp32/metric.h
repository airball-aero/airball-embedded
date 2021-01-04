#ifndef METRIC_H
#define METRIC_H

class Metric {
 public:
 Metric() : n_(0L), min_(0.0f), max_(0.0f), tot_(0.0f) {}
  ~Metric() {}

  void add(float x) {
    lat_ = x;
    tot_ += x;
    if (n_ == 0L) { min_ = max_ = x; }
    if (x < min_) { min_ = x; }
    if (x > max_) { max_ = x; }
    n_++;
  }

  const char* str() {
    sprintf(buf_,
	    "n,%ld,min,%f,max,%f,avg,%f,lat,%f",
	    n_,
	    min_,
	    max_,
	    tot_ / ((float) n_),
	    lat_);
    return buf_;
  }
  
 private:
  char buf_[1024];
  long n_;
  float tot_;
  float min_;
  float max_;
  float lat_;
};

#endif
