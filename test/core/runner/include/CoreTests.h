#pragma once

#include <gtest/gtest.h>

#include <filesystem>

namespace vanadium::e2e::core {

class SnapshotTest : public ::testing::Test {
 public:
  SnapshotTest(std::filesystem::path suite, std::filesystem::path file)
      : suite_(std::move(suite)), file_(std::move(file)) {};

  void TestBody() override;

 private:
  const std::filesystem::path suite_;
  const std::filesystem::path file_;
};

}  // namespace vanadium::e2e::core
