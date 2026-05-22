cmake_minimum_required(VERSION 2.8.12.2) 

link_directories($(ACE_ROOT)/ace)

add_library(xglog
xglog.cpp
LogProfile.cpp)

target_link_libraries(xglog -lACE)
