enable_testing()

add_executable(
  sfs_test
  # flash_mock_test.cpp
  sfs_test.cpp
)

target_link_libraries(
  sfs_test
  GTest::gtest_main
  flash_mock
  sfs
)

target_compile_options(sfs_test PRIVATE -Wall -Wextra -Wpedantic)
include(GoogleTest)
gtest_discover_tests(sfs_test)
