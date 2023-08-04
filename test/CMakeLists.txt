enable_testing()

add_executable(
  hello_test
  hello_test.cpp
)

target_link_libraries(
  hello_test
  GTest::gtest_main
  flash_mock
)

include(GoogleTest)
gtest_discover_tests(hello_test)
