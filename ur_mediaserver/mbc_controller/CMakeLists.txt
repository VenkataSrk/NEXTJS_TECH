cmake_minimum_required(VERSION 2.8.12.2) 

link_directories($(ACE_ROOT)/ace)

add_library(mbc_controller
MBCController.cpp)

target_link_libraries(mbc_controller -lACE)
