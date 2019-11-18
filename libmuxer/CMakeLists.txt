cmake_minimum_required(VERSION 3.15)

aux_source_directory(./ DIR_LIB_SRCS)
include_directories(./libffmpeg/include)
include_directories(./libx264/include)
include_directories(./libfdk-aac/include)
include_directories(../utils/libspdlog/include)
link_directories(./libffmpeg/lib ./libfdk-aac/lib ./libx264/lib ../utils/libspdlog/lib)

add_library(muxer STATIC ${DIR_LIB_SRCS})
target_link_libraries(muxer)