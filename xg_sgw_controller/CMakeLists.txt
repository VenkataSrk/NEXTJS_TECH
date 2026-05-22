cmake_minimum_required(VERSION 3.5)


link_directories($(ACE_ROOT)/ace)

add_library(xgsgwcontroller
xGateSGWController.cpp)

target_link_libraries(xgsgwcontroller -lACE)
