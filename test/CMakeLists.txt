enable_testing()

add_executable(
  sfs_test
  flash_mock_test.cpp
)

target_link_libraries(
  sfs_test
  GTest::gtest_main
  flash_mock
)

include(GoogleTest)
gtest_discover_tests(sfs_test)
