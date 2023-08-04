#include <gtest/gtest.h>

extern "C" {
    #include "flash_mock/flash_mock.h"
}

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
}

TEST(HelloTest, CheckMock) {
  EXPECT_EQ(13, flash_mock_test());
}