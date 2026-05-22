cmake_minimum_required(VERSION 3.5)

link_directories($(ACE_ROOT)/ace)

add_library(xgdbservice
xGateDBService.cpp
xGateDBHandler.cpp
xGateDBConnector.cpp)

target_link_libraries(xgdbservice -lACE -lodbc)
