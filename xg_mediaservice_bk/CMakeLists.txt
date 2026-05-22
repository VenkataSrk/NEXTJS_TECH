cmake_minimum_required(VERSION 2.8.12.2) 

link_directories($(ACE_ROOT)/ace)

add_library(xgmediaservice
xGateMediaService.cpp
MediaServiceUtil.cpp
MediaTcpReceiver.cpp
MediaTcpSocketHandler.cpp
MediaTcpAcceptor.cpp
MediaMsgDispatcher.cpp
MediaMsgProcessor.cpp)

target_link_libraries(xgmediaservice -lACE)
