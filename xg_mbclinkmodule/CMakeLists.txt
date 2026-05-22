cmake_minimum_required(VERSION 2.8.12.2) 

link_directories($(ACE_ROOT)/ace)

add_library(xgmbclinkmodule
xGateMBCLinkService.cpp
MBCLinkServiceUtil.cpp
MBCLinkTcpReceiver.cpp
MBCLinkTcpSocketHandler.cpp
MBCLinkTcpAcceptor.cpp
MBCLinkMsgDispatcher.cpp
MBCLinkMsgProcessor.cpp)

target_link_libraries(xgmbclinkmodule -lACE)
