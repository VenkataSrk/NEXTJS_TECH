cmake_minimum_required(VERSION 2.8.12.2)

link_directories($(ACE_ROOT)/ace)

add_library(xg_dbservice 
xGateDBConnector.cpp
xGateDBHandler.cpp
xGateDBService.cpp)

target_link_libraries(xg_dbservice -lACE -lodbc)
