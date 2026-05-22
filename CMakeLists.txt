cmake_minimum_required(VERSION 2.8.12.2 FATAL_ERROR) 

set(NAME uravconf)
project(${NAME})

set(CMAKE_CXX_FLAGS "-std=c++17")
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include_directories(/usr/include)
include_directories(/usr/include/curl)
include_directories(/usr/include/netinet)
include_directories(/usr/local/src)
include_directories(/usr/local/lib)
include_directories(/usr/local/lib64)
include_directories(/usr/local/)
include_directories(/usr/local/include)
include_directories(/usr/local/include/sofia-sip-1.13)
include_directories(/usr/local/include/sofia-sip-1.13/sofia-sip)
include_directories(/usr/local/include/hiredis)
include_directories(/usr/local/include/sw/redis++)
include_directories(/usr/local/include/curl)
include_directories(/usr/local/xgate/ACE_wrappers)
include_directories(/usr/local/xgate/ACE_wrappers/ace)
include_directories(rapidjson)
include_directories(rapidxml)
include_directories(xg_logger)
include_directories(xg_reactor)
include_directories(xg_parser)
include_directories(xg_iurmodule)
include_directories(inc)
include_directories(xg_schedulerservice)
include_directories(urconfbridge_controller)
include_directories(xg_sipservice)
include_directories(xg_mbclinkmodule)
include_directories(xg_redisservice)

add_subdirectory(xg_logger)
add_subdirectory(xg_schedulerservice)
add_subdirectory(urconfbridge_controller)
add_subdirectory(xg_sipservice)
add_subdirectory(xg_mbclinkmodule)
add_subdirectory(xg_redisservice)

link_directories(
/usr/lib
/usr/lib64
/usr/local/lib
/usr/local/lib64
/usr/local/xgate/ACE_wrappers/
/usr/local/xgate/ACE_wrappers/ace
)

add_executable(${NAME}
src/main.cpp
src/UREngineConfig.cpp
src/xGateUtil.cpp
xg_parser/xGateRapidXML.cpp
xg_parser/xGateXMLParser.cpp
xg_reactor/xGateScheduler.cpp
xg_reactor/ReadConfigFromProxyConfFile.cpp
urconfbridge_controller/URCBController.cpp
xg_iurmodule/IURModuleCallBack.cpp
xg_iurmodule/IURModuleConfig.cpp
xg_iurmodule/IURModule.cpp
xg_iurmodule/IURModuleMsg.cpp)

target_link_libraries(${NAME}
xglog
xgschedulerservice
urconfbridgecontroller
xgsipservice
xgmbclinkmodule
xgredisservice
ACE
curl
uuid
pthread
dl
rt
m
sofia-sip-ua
ssl
crypto
)

#target_link_libraries(${NAME} -lACE -lcurl -luuid -lsofia-sip-ua -lssl -lcrypto -lpthread)
#target_link_libraries(${NAME} -lACE -lcurl -lpthread)
#target_link_libraries(${NAME} xglog)
#target_link_libraries(${NAME} xgschedulerservice)
#target_link_libraries(${NAME} urconfbridgecontroller)
#target_link_libraries(${NAME} xgsipservice)
#target_link_libraries(${NAME} xgmbclinkmodule)
#target_link_libraries(${NAME} xgredisservice)

install(CODE
"IF(EXISTS \"/etc/systemd/system/uravconf.service\")
  message(STATUS \"Service file(uravconf.service) - found\")
ELSE()
  message(AUTHOR_WARNING \"Service file(uravconf.service) - not found\")
  message(STATUS \"Service file(uravconf.service) - copying ...\")
  file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/uravconf.service DESTINATION /lib/systemd/system)
  file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/uravconf.service DESTINATION /etc/systemd/system)
  message(STATUS \"Service file(uravconf.service) - copied\")
ENDIF()")

install(TARGETS ${NAME} DESTINATION /usr/bin)
install(CODE "execute_process(COMMAND mkdir -p /var/log/xgate/pbx)")
install(CODE "execute_process(COMMAND systemctl daemon-reload)")
install(CODE "execute_process(COMMAND systemctl enable uravconf.service)")
install(CODE "execute_process(COMMAND systemctl restart uravconf.service)
message(STATUS \"Service file(uravconf.service) ==> STARTED\")")

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/start_uravconf DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
message(STATUS "Conf Bridge :- Copied start_uravconf file from ${CMAKE_CURRENT_SOURCE_DIR} to ${CMAKE_CURRENT_BINARY_DIR}")

add_custom_command(TARGET ${NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E echo "${NAME} has been compiled")
