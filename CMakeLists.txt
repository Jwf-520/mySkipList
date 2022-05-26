cmake_minimum_required(VERSION 3.2)
project(skiplist)
set(CMAKE_BUILD_TYPE DEBUG_CONFIGURATIONS)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
include_directories(
    ${PROJECT_SOURCE_DIR}/include
)
add_executable(skiplist src/main.cpp)
target_link_libraries(skiplist pthread)

