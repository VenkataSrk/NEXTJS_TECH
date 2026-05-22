cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

set(NAME urengine_wss)
project(${NAME})

set(CMAKE_CXX_FLAGS "-std=c++20")
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

#include_directories(/usr/local/include/sofia-sip-1.12)
include_directories(/usr/local/include/sofia-sip-1.13)
include_directories(/usr/local/include/sofia-sip-1.13/sofia-sip)
include_directories(/usr/local/include/)
include_directories(rapidjson)
include_directories(rapidxml)
include_directories(xg_logger)
include_directories(xg_iurmodule)
include_directories(xg_json)
include_directories(inc)
include_directories(xg_schedulerservice)
include_directories(xg_sgw_controller)
include_directories(xg_sipservice)
include_directories(xg_netioservice)
include_directories(xg_dbservice)
include_directories(xg_monitorservice)
link_directories(/usr/local/lib)

add_subdirectory(xg_logger)
add_subdirectory(xg_schedulerservice)
add_subdirectory(xg_sgw_controller)
add_subdirectory(xg_sipservice)
add_subdirectory(xg_netioservice)
add_subdirectory(xg_dbservice)
add_subdirectory(xg_monitorservice)

add_executable(${NAME}
src/main.cpp
src/UREngineConfig.cpp
src/xGateUtil.cpp
src/xGateMultiSessionTracker.cpp
xg_json/xGateRapidJsonAdaptor.cpp
xg_sgw_controller/xGateSGWController.cpp
xg_iurmodule/IURModuleCallBack.cpp
xg_iurmodule/IURModuleConfig.cpp
xg_iurmodule/IURModule.cpp
xg_iurmodule/IURModuleMsg.cpp)

target_link_libraries(${NAME} -lACE -lsofia-sip-ua -lpthread -luuid -lodbc)
target_link_libraries(${NAME} xglog)
target_link_libraries(${NAME} xgschedulerservice)
target_link_libraries(${NAME} xgsgwcontroller)
target_link_libraries(${NAME} xgsipservice)
target_link_libraries(${NAME} xgnetioservice)
target_link_libraries(${NAME} xgdbservice) 
target_link_libraries(${NAME} xgmonitorservice) 

install(CODE
"IF(EXISTS \"/etc/systemd/system/urengine_wss.service\")
  message(STATUS \"Service file(urengine_wss.service) - found\")
ELSE()
  message(AUTHOR_WARNING \"Service file(urengine_wss.service) - not found\")
  message(STATUS \"Service file(urengine_wss.service) - copying ...\")
  file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/urengine_wss.service DESTINATION /lib/systemd/system)
  file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/urengine_wss.service DESTINATION /etc/systemd/system)
  message(STATUS \"Service file(urengine_wss.service) - copied\")
ENDIF()")

install(TARGETS ${NAME} DESTINATION /usr/bin)
install(CODE "execute_process(COMMAND mkdir -p /var/log/xgate/wss)")
install(CODE "execute_process(COMMAND systemctl daemon-reload)")
install(CODE "execute_process(COMMAND systemctl enable urengine_wss.service)")
install(CODE "execute_process(COMMAND systemctl restart urengine_wss.service)
message(STATUS \"Service file(urengine_wss.service) ==> STARTED\")")

set_property(TARGET ${NAME} APPEND PROPERTY ADDITIONAL_CLEAN_FILES /usr/bin/urengine_wss
/var/log/xgate/wss/urengine_wss.out /var/log/xgate/wss/urengine_wss.err)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/start_wss DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
message(STATUS "SGW :- Copied start_wss file from ${CMAKE_CURRENT_SOURCE_DIR} to ${CMAKE_CURRENT_BINARY_DIR}")
