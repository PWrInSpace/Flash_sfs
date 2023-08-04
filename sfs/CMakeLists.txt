add_library(sfs tools.cpp)
target_link_libraries(sfs PUBLIC ${PROJECT_NAME}_setup)