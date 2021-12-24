cmake_minimum_required(VERSION 3.16)

set(PROJECT convarm)
project(${PROJECT})

include_directories(include)
file(GLOB SOURCES source/*.cpp)

add_executable(${PROJECT} ${SOURCES})
target_link_libraries(${PROJECT} fmt)