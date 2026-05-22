cmake_minimum_required(VERSION 3.23.1 FATAL_ERROR)

set(NAME srs)
project(${NAME})

set(CMAKE_CXX_FLAGS "-std=c++2a -w -fpermissive -z muldefs")
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_STANDARD 20)


find_package(PkgConfig)
if (PKG_CONFIG_FOUND)
  pkg_check_modules(GSTREAMER "gstreamer-1.0")
  if (GSTREAMER)
    target_link_libraries(${NAME} GSTREAMER_LIBRARIES)
    add_definitions(${GSTREAMER_CFLAGS} ${GSTREAMER_CFLAGS_OTHER})
  endif()
  pkg_check_modules(GLIB "glib-2.0")
  if (GLIB)
    target_link_libraries(${NAME} GLIB_LIBRARIES)
    add_definitions(${GLIB_CFLAGS} ${GLIB_CFLAGS_OTHER})
  endif()
  pkg_check_modules(GSTREAMERRTP "gstreamer-rtp-1.0")
  if (GSTREAMERRTP)
    target_link_libraries(${NAME} GSTREAMERRTP_LIBRARIES)
    add_definitions(${GSTREAMERRTP_CFLAGS} ${GSTREAMERRTP_CFLAGS_OTHER})
  endif()
  pkg_check_modules(GSTREAMERAPP "gstreamer-app-1.0")
  if (GSTREAMERAPP)
    target_link_libraries(${NAME} GSTREAMERAPP_LIBRARIES)
    add_definitions(${GSTREAMERAPP_CFLAGS} ${GSTREAMERAPP_CFLAGS_OTHER})
  endif()
  pkg_check_modules(GSTREAMERAPP "gstreamer-base-1.0")
  if (GSTREAMERAPP)
    target_link_libraries(${NAME} GSTREAMERAPP_LIBRARIES )
    add_definitions(${GSTREAMERAPP_CFLAGS} ${GSTREAMERAPP_CFLAGS_OTHER})
  endif()
endif()

include_directories(${GLIB_INCLUDE_DIRS} ${GSTREAMER_INCLUDE_DIRS} ${GIO_INCLUDE_DIRS})
include_directories(./include)
include_directories(/usr/include)
include_directories(/usr/local/include)
include_directories(/usr/local/src/ACE_wrappers)
include_directories(/usr/local/include/gstreamer-1.0)
include_directories(/usr/local/include/glib-2.0)
include_directories(/usr/local/lib/x86_64-linux-gnu/glib-2.0/include)
#include_directories(/usr/local/lib64/glib-2.0/include)
include_directories(./include/rapidjson)

link_directories(${GLIB_LIBRARY_DIRS} ${GSTREAMER_LIBRARY_DIRS} ${GIO_LIBRARY_DIRS} /usr/local/lib /usr/local/lib64)


include_directories(/usr/local/include/sofia-sip-1.13)
include_directories(/usr/include/curl)
include_directories(/usr/local/include/curl)
include_directories(/usr/include/netinet)
include_directories(/usr/local/lib)
include_directories(/usr/local/src)
include_directories(/usr/local/include/gstreamer-1.0)
include_directories(/usr/local/include/glib-2.0)
include_directories(/usr/local/lib64/glib-2.0/include)
include_directories(xgatesip/xgate_xmlparser)
include_directories(/usr/include)
include_directories(/usr/local/include)
include_directories(rapidjson)
include_directories(rapidxml-1.13)
include_directories(src)
include_directories(inc)
include_directories(verifications)
include_directories(xmlparser)
include_directories(xg_iurmodule)
include_directories(xgate_reactor)
include_directories(xgate_media)
include_directories(xgatesip)
include_directories(xg_httpservice)
include_directories(xg_dbservice)
include_directories(xg_logger)
include_directories(ur_modulebridge)
include_directories(xg_redisservice)
include_directories(/usr/local/src/ACE_wrappers)

link_directories(${GLIB_LIBRARY_DIRS} ${GSTREAMER_LIBRARY_DIRS} ${GIO_LIBRARY_DIRS} /usr/local/lib /usr/local/lib64)
add_executable(${NAME}
./src/main.cpp
./src/xGConfig.cpp
./xgate_reactor/xGateUtil.cpp
./xgatesip/xgate_xmlparser/xGateRapidXML.cpp
./xgatesip/xgate_xmlparser/xGateRSXMLParserUtil.cpp
./xgatesip/xgate_xmlparser/xGateRSXMLParser.cpp
./xgate_reactor/xGateScheduler.cpp
./xgate_reactor/xGateProcessor.cpp
./xgate_reactor/xGateSrvrProcessor.cpp
./xgate_reactor/xGateTCPServer.cpp
./xgate_reactor/xGateSrvAcceptor.cpp
./xgate_reactor/xGateTCPReceiver.cpp
./xgate_reactor/xGateConnector.cpp
./xgate_reactor/xGateTCPSocketHandler.cpp
./xgate_media/xGateHmpGstUtil.cpp
./xgate_media/xGateHmpGstAudioBin.cpp
./xgate_media/xGateHmpGstVideoBin.cpp
./xgate_media/xGateHmpGstBin.cpp
./xgate_media/xGateHmpGstManager.cpp
./xgate_media/xGateHmpProcessor.cpp
./xgatesip/SofiaSiphandler.cpp
./xgatesip/xGateSipSignalDispatcher.cpp
./xgatesip/xGateSipSignalReceiver.cpp
./xgatesip/xGateSipSignalProcessor.cpp
./xgatesip/xGateSipController.cpp
./xgatesip/xGateSipRecording.cpp
./xgatesip/xGateCallInfoEncoder.cpp
./xgate_reactor/xGateDispatcher.cpp
./xgate_reactor/xGateMGDispatcher.cpp
./ur_modulebridge/URModuleBridge.cpp
./ur_modulebridge/URModuleBridge.h
./xg_iurmodule/IURModuleCallBack.cpp
./xg_iurmodule/IURModuleConfig.cpp	
./xg_iurmodule/IURModule.cpp
./xg_iurmodule/IURModuleMsg.cpp
./xg_httpservice/xGateHttpService.cpp
./xg_httpservice/xGateHttpHandler.cpp
./xg_dbservice/xGateDBService.cpp
./xg_dbservice/xGateDBHandler.cpp
./xg_dbservice/xGateDBConnector.cpp
./xg_redisservice/xGateRedisService.cpp
./xg_redisservice/xGateRedisHandler.cpp
./xg_logger/xglog.cpp
./xg_logger/LogProfile.cpp)

target_link_libraries(${NAME} -lsofia-sip-ua -lodbc -lpthread -lpt -lcurl -lgio-2.0 -lgobject-2.0 -lglib-2.0 -lgmodule-2.0 -lACE /usr/local/lib/libredis++.a -lhiredis -lredis++ gstreamer-1.0)

install(CODE
"IF(EXISTS \"/etc/systemd/system/srs.service\")
  message(STATUS \"Service file(srs.service) - found\")
ELSE()
  message(AUTHOR_WARNING \"Service file(srs.service) - not found\")
  message(STATUS \"Service file(srs.service) - copying ...\")
  file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/srs.service DESTINATION /lib/systemd/system)
  file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/srs.service DESTINATION /etc/systemd/system)
  message(STATUS \"Service file(srs.service) - copied\")
ENDIF()")

install(TARGETS ${NAME} DESTINATION /usr/bin)
install(CODE "execute_process(COMMAND mkdir -p /var/log/xgate/srs)")
install(CODE "execute_process(COMMAND systemctl daemon-reload)")
install(CODE "execute_process(COMMAND systemctl enable srs.service)")
install(CODE "execute_process(COMMAND systemctl restart srs.service)
message(STATUS \"Service file(srs.service) ==> STARTED\")")

set_property(TARGET ${NAME} APPEND PROPERTY ADDITIONAL_CLEAN_FILES /usr/bin/srs
/var/log/xgate/srs/srs.out /var/log/xgate/srs/srs.err)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/start_srs DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
message(STATUS "SRS :- Copied start_srs file from ${CMAKE_CURRENT_SOURCE_DIR} to ${CMAKE_CURRENT_BINARY_DIR}")
