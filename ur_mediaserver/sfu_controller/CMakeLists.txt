cmake_minimum_required(VERSION 2.8.12.2) 

link_directories($(ACE_ROOT)/ace)

include_directories(../xg_mediaservice/include)
include_directories(../xg_sfu/include)
include_directories(../xg_iurdefines)
include_directories(../xg_mbc/include)

add_library(sfu_controller
XG_SFUController.cpp)

target_link_libraries(sfu_controller -lACE)
