cmake_minimum_required(VERSION 2.8.12.2)


link_directories($(ACE_ROOT)/ace)

add_library(urconfbridgecontroller
URCBController.cpp
URConfRoomContextHandler.cpp
URCBCallContextHandler.cpp)

target_link_libraries(urconfbridgecontroller -lACE -luuid)
