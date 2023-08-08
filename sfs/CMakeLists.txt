add_library(sfs simple_file_system.c)
target_link_libraries(sfs PUBLIC ${PROJECT_NAME}_setup)