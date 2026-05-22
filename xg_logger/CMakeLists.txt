cmake_minimum_required(VERSION 3.5)

link_directories($(ACE_ROOT)/ace)

add_library(xglog
xglog.cpp
LogProfile.cpp)

target_link_libraries(xglog -lACE)
