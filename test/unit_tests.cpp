#include "program_options.h"
#include "gtest/gtest.h"

#include <iostream>

TEST(ProgramOptionTest, NormalTest) {
  auto check_valid = [](int val) -> bool { return val > 5; };

  ProgramOptions po;
  po.register_option<int>("-a", "a");
  po.register_option<int>("-b", "b", 5);
  po.register_option<int>("-c", "c", check_valid);
  po.register_option<int>("-d", "d", 10, check_valid);
}

TEST(ProgramOptionTest, NoRegisteOptionsTest) {
  const char *cmd[]{"app", "-a", "5", "-b", "4", "-c", "cc"};

  ProgramOptions po;
  EXPECT_THROW(po.read(7, cmd), std::runtime_error);
}

TEST(ProgramOptionTest, ReadDuplicatedOptionTest) {
  const char *cmd[]{"app", "-a", "5", "-a", "4"};

  ProgramOptions po;
  po.register_option<int>("-a", "aaa");
  EXPECT_THROW(po.read(5, cmd), std::runtime_error);
}

TEST(ProgramOptionTest, DuplicatedNameTest) {
  ProgramOptions po;
  po.register_option<int>("-a", "a");
  EXPECT_THROW(po.register_option<int>("-a", "a"), std::runtime_error);
}

TEST(ProgramOptionTest, InvalidValueTest) {
  auto check_valid = [](int val) -> bool { return val > 5; };

  ProgramOptions po;
  EXPECT_THROW(po.register_option<int>("-d", "d", 1, check_valid),
               std::runtime_error);
}

TEST(ProgramOptionTest, GetDiffTypeValueTest) {
  const char *cmd[]{"app", "-a", "5"};

  ProgramOptions po;
  po.register_option<int>("-a", "a");
  po.read(3, cmd);
  EXPECT_THROW(po.get<float>("-a"), std::runtime_error);
}

GTEST_API_ int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
