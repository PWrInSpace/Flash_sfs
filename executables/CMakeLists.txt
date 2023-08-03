add_executable(print_hello main.cpp)
target_link_libraries(print_hello PRIVATE tools ${PROJECT_NAME}_setup)