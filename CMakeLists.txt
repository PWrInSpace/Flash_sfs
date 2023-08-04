cmake_minimum_required(VERSION 3.16)
project(test)

add_library(${PROJECT_NAME}_setup INTERFACE)
target_compile_options(${PROJECT_NAME}_setup INTERFACE -Wall -Wpedantic -Wextra)
target_include_directories(${PROJECT_NAME}_setup INTERFACE ${PROJECT_SOURCE_DIR}) 


# Google Test requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)
    
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    
add_subdirectory(test)
add_subdirectory(sfs)
add_subdirectory(flash_mock)
add_subdirectory(executables)

enable_testing()
