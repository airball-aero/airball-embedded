#include <memory>
#include <gtest/gtest.h>
#include "parameter_file.h"

constexpr char* path_template = "/tmp/parameterfileXXXXXX";

const airball::ParameterFile::Parameter<int> TEST_PARAM(
    "test_param",
    42);

class ParameterFileTest : public testing::Test {
protected:
  void SetUp() override {
    char path[strlen(path_template) + 1];
    strcpy(path, path_template);
    mktemp(path);
    if (strlen(path) == 0) {
      FAIL();
    }
    path_ = path;
  }

  void Open() {
    f_.reset(new airball::ParameterFile(path_));
  }

  void Close() {
    f_.reset(nullptr);
  }

  std::string path_;
  std::unique_ptr<airball::ParameterFile> f_;
};

TEST_F(ParameterFileTest, StandardBehavior) {
  Open();
  EXPECT_EQ(f_->get_value(&TEST_PARAM), 42);
  f_->set_value(&TEST_PARAM, 55);
  EXPECT_EQ(f_->get_value(&TEST_PARAM), 55);
  Close();
  Open();
  EXPECT_EQ(f_->get_value(&TEST_PARAM), 42);
  f_->set_value(&TEST_PARAM, 55, true);
  EXPECT_EQ(f_->get_value(&TEST_PARAM), 55);
  Close();
  Open();
  EXPECT_EQ(f_->get_value(&TEST_PARAM), 55);
  f_->set_value(&TEST_PARAM, 66);
  f_->save();
  Close();
  Open();
  EXPECT_EQ(f_->get_value(&TEST_PARAM), 66);
}
