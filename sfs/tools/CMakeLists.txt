add_library(tools tools.cpp)
target_link_libraries(tools PUBLIC ${PROJECT_NAME}_setup)