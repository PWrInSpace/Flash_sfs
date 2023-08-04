add_library(flash_mock flash_mock.c)
target_link_libraries(flash_mock PUBLIC ${PROJECT_NAME}_setup)