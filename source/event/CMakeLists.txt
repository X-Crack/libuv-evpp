cmake_minimum_required(VERSION 3.17)
aux_source_directory(include DIR_EVENT_SRCS)
add_library(event ${DIR_EVENT_SRCS})