cmake_minimum_required(VERSION 2.8.12.2) 

link_directories($(ACE_ROOT)/ace)

set(CMAKE_CXX_FLAGS "-std=c++0x -fpermissive -z muldefs `pkg-config --cflags --libs gstreamer-1.0` `pkg-config --cflags --libs glib-2.0` -DH248_MG -DXGATE_HAS_GSTREAMER")

include_directories("include")

add_library(xgmbc
src/xGateMBCUtil.cpp
src/ClientService.cpp
src/xGateDispatcher.cpp
src/xGateMgSFUDispatcher.cpp
src/MBCSessionHandler.cpp
src/MBCControllerProcessor.cpp
src/xGateMBCService.cpp)

target_link_libraries(xgmbc -lACE)
