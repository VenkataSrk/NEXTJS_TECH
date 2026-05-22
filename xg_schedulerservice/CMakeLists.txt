cmake_minimum_required(VERSION 3.5)


link_directories($(ACE_ROOT)/ace)

add_library(xgschedulerservice
xGateScheduler.cpp
xGateSchedulerService.cpp
xGateSchedulerHandler.cpp
SchedulerContext.cpp)

target_link_libraries(xgschedulerservice -lACE)
