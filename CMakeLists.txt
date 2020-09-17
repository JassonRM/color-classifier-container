cmake_minimum_required(VERSION 3.17)
project(color_classifier_container C)

set(CMAKE_C_STANDARD 99)

add_executable(color_classifier_container src/main.c src/image.c)

find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIR})
target_link_libraries(color_classifier_container ${PNG_LIBRARY})