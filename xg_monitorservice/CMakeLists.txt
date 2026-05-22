cmake_minimum_required(VERSION 3.5)

link_directories($(ACE_ROOT)/ace)

add_library(xgmonitorservice
xGateMonitorService.cpp
xGateMonitorHandler.cpp
MonitorCtxHandler.cpp)

target_link_libraries(xgmonitorservice -lACE)
