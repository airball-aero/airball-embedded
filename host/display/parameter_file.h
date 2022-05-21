#ifndef PARAMETER_FILE_H
#define PARAMETER_FILE_H

#include <string>
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <iostream>

namespace airball {

class ParameterFile {
public:
  explicit ParameterFile(const std::string& path);
  virtual ~ParameterFile() = default;

  void load();
  void save() const;

  template<class T>
  class Parameter {
  public:
    Parameter(const std::string& name, const T& initial)
        : name_(name), initial_(initial) {}

    T get(const rapidjson::Document &doc) const {
      if (doc.HasMember(name_.c_str())) {
        return get_impl(doc);
      }
      return initial_;
    }

    void set(rapidjson::Document &doc, T value) const {
      set_impl(doc, value);
    }

    T get_impl(const rapidjson::Document &doc) const;

    void set_impl(rapidjson::Document &doc, const T& value) const {
      if (!doc.HasMember(name_.c_str())) {
        doc.AddMember(
            rapidjson::StringRef(name_.c_str()),
            value,
            doc.GetAllocator());
      } else {
        doc[name_.c_str()].Set(value);
      }
    }

  private:
    const std::string name_;
    const T initial_;
  };

  template<class T>
  T get_value(
      const Parameter<T> *p) const {
    return p->get(document_);
  }

  template<class T>
  void set_value(
      const Parameter<T> *p,
      const T& value) {
    p->set(document_, value);
  }

  template<class T>
  void set_value(
      const Parameter<T> *p,
      const T& value,
      bool save_on_write) {
    set_value(p, value);
    if (save_on_write) {
      save();
    }
  }

private:

  std::string to_string() const;

  const std::string path_;
  rapidjson::Document document_;
};

} // namespace airball

#endif  // PARAMETER_FILE_H
