add_executable(print_hello main.cpp)
target_link_libraries(print_hello PRIVATE 
                        sfs flash_mock ${PROJECT_NAME}_setup)