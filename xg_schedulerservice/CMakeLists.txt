cmake_minimum_required(VERSION 2.8.12.2) 


link_directories($(ACE_ROOT)/ace)

add_library(xgschedulerservice
xGateSchedulerService.cpp
xGateSchedulerHandler.cpp
SchedulerContext.cpp)

target_link_libraries(xgschedulerservice -lACE)
