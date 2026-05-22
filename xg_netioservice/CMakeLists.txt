cmake_minimum_required(VERSION 3.5)

link_directories($(ACE_ROOT)/ace)

add_library(xgnetioservice
xGateNetIOService.cpp
NetIOServiceUtil.cpp
TcpReceiver.cpp
TcpSocketHandler.cpp
TcpAcceptor.cpp
Connector.cpp
TcpConnector.cpp
NetIOMsgDispatcher.cpp
NetIOMsgProcessor.cpp)

target_link_libraries(xgnetioservice -lACE)
