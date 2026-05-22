cmake_minimum_required(VERSION 2.8.12.2)

link_directories($(ACE_ROOT)/ace)
link_directories(/usr/local/lib/redis)

add_library(xgredisservice
xGateRedisService.cpp
xGateRedisHandler.cpp)

target_link_libraries(xgredisservice -lACE -lhiredis -lredis++)
