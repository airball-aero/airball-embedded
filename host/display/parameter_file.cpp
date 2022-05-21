#include "parameter_file.h"

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include <fstream>
#include <sstream>

namespace airball {

template<>
int ParameterFile::Parameter<int>::get_impl(
    const rapidjson::Document &doc) const {
  return doc[name_.c_str()].GetInt();
}

template<>
double ParameterFile::Parameter<double>::get_impl(
    const rapidjson::Document &doc) const {
  return doc[name_.c_str()].GetDouble();
}

template<>
bool ParameterFile::Parameter<bool>::get_impl(
    const rapidjson::Document &doc) const {
  return doc[name_.c_str()].GetBool();
}

template<>
std::string ParameterFile::Parameter<std::string>::get_impl(
    const rapidjson::Document &doc) const {
  return doc[name_.c_str()].GetString();
}

ParameterFile::ParameterFile(const std::string& path)
    : path_(path) {
  document_.Parse("{}");
}

void ParameterFile::load() {
  std::ifstream f;
  f.open(path_);
  std::stringstream s;
  s << f.rdbuf();
  f.close();
  document_.Parse("{}");
  document_.Parse(s.str().c_str());
}

std::string d2s(const rapidjson::Document& doc) {
  rapidjson::StringBuffer buffer;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
  doc.Accept(writer);
  return buffer.GetString();
}

std::string ParameterFile::to_string() const {
  rapidjson::StringBuffer buffer;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
  document_.Accept(writer);
  return buffer.GetString();
}

void ParameterFile::save() const {
  std::string s = to_string();
  std::ofstream f;
  f.open(path_);
  f.write(s.c_str(), s.length());
  f.close();
}

} // namespace airball

