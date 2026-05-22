cmake_minimum_required(VERSION 2.8.12.2) 

link_directories($(ACE_ROOT)/ace)

include_directories(/usr/local/include/sofia-sip-1.12)
include_directories()

add_library(xgsipservice
xGateSipService.cpp
xGateSipController.cpp
SofiaSiphandler.cpp
xGateSipSignalReceiver.cpp
xGateDispatcher.cpp
xGateSipSignalDispatcher.cpp
xGateProcessor.cpp
xGateSipSignalProcessor.cpp
SipMediahandler.cpp)

target_link_libraries(xgsipservice -lsofia-sip-ua -lACE)
