cmake_minimum_required(VERSION 3.15)
project(pc-codec)

set(CMAKE_CXX_STANDARD 14)
include_directories(./utils/libspdlog/include)
add_subdirectory(libmuxer)
#link_directories(./utils/libspdlog/lib)
add_executable(pc-codec main.cpp)
target_link_libraries(pc-codec muxer)