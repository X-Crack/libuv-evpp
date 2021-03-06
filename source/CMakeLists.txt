cmake_minimum_required(VERSION 3.17)
aux_source_directory(include DIR_SOURCE_SRCS)
add_library(source ${DIR_SOURCE_SRCS})