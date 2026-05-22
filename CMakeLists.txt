cmake_minimum_required(VERSION 3.5)

set(NAME stt_tts_mg)
project(${NAME})

set(CMAKE_CXX_FLAGS "-std=c++2a -w -fpermissive -z muldefs -DH248_MG")
#set(CMAKE_CXX_FLAGS "-std=c++11 -w -fpermissive -z muldefs -DH248_MG")
set(CMAKE_CXX_FLAGS_DEBUG "-g -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_STANDARD 20)

#set(CFLAGS "-g  -DNDEBUG  -w -fpermissive -z muldefs  -DH248_MG")
#set(LFLAGS "/usr/local/lib $(ACE_ROOT)/ace /usr/local/apr/lib /usr/local/apr/lib/apr-util-1 /usr/local/lib64")
set(LIBS "-lpthread -lpt -lrt -lm -lssl -lcrypto -lgio-2.0 -lgobject-2.0 -lglib-2.0 -lgmodule-2.0 -lACE  -lhiredis -lredis++ -lunimrcpclient -lapr-1 -laprutil-1 -lcurl -lboost_system -lboost_thread")

# MSVC requires some additional code to select the correct runtime library
if (VCPKG_TARGET_TRIPLET MATCHES "-static$")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
else ()
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif ()


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


#pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)
include_directories(${GLIB_INCLUDE_DIRS} ${GSTREAMER_INCLUDE_DIRS} ${GIO_INCLUDE_DIRS})
include_directories(${ACE_ROOT})
include_directories(/opt/boost/include)
include_directories(/usr/include)
include_directories(include)
include_directories(/usr/local/include)
include_directories(/usr/local/include/gstreamer-1.0)
include_directories(/usr/local/include/glib-2.0)
include_directories(/usr/local/lib64/glib-2.0/include)
include_directories(/usr/local/unimrcp/include)
include_directories(/usr/local/apr/include)
include_directories(/usr/local/apr/include/apr-1)
include_directories(xg_logger)
include_directories(xg_schedulerservice)
include_directories(ur_mrcp_client/inc)
include_directories(rapidxml)
include_directories(rapidjson)
include_directories(ur_bridgemodule)
include_directories(xg_httpservice)
include_directories(xg_iurmodule)
include_directories(xg_redisservice)
include_directories(./include)
include_directories(/usr/local/src/ACE_wrappers)

link_directories(${GLIB_LIBRARY_DIRS} ${GSTREAMER_LIBRARY_DIRS} ${GIO_LIBRARY_DIRS} /usr/local/lib ${ACE_ROOT}/ace /usr/local/lib64 /usr/local/apr/lib /usr/local/apr/lib/apr-util-1 /usr/local/unimrcp/lib /opt/boost/lib)

add_executable(${NAME}
./src/main.cpp
./src/xGateAuditUtil.cpp
./src/xGateAuditService.cpp
./src/xGateAuditManager.cpp
./src/xGateAuditHandler.cpp
./src/xGateAuditScheduler.cpp
./src/xGateScheduler.cpp
./src/xGateUtil.cpp
./src/xGateHmpGstUtil.cpp
./src/xGateHmpGstAudioBin.cpp
./src/xGateHmpGstBin.cpp
./src/xGateHmpGstManager.cpp
./src/xGateProcessor.cpp
./src/xGateMgProcessor.cpp
./src/xGateDispatcher.cpp
./src/xGateMGDispatcher.cpp
./src/xGateTCPServer.cpp
./src/xGateTCPReceiver.cpp
./src/xGateTCPReceiver.cpp
./src/xGateConnector.cpp
./src/xGateMGCConnector.cpp
./src/xGateTCPSocketHandler.cpp
./src/xGateHmpProcessor.cpp
./src/xGateUtil.cpp
./src/xGateServerConfig.cpp
./src/stringencode.cpp
./src/Base64.cpp
./xg_logger/xglog.cpp
./xg_logger/LogProfile.cpp
./xg_iurmodule/IURModuleCallBack.cpp
./xg_iurmodule/IURModuleConfig.cpp
./xg_iurmodule/IURModule.cpp
./xg_iurmodule/IURModuleMsg.cpp
./xg_redisservice/MrcpRedisDbConnector.cpp
./xg_redisservice/MrcpRedisDbSubscriber.cpp
./ur_bridgemodule/URMRCPModuleBridge.cpp
./ur_bridgemodule/MrcpConfig.cpp
./ur_mrcp_client/src/MrcpCliAppSpeechRecognizeScenario.cpp
./ur_mrcp_client/src/MrcpCliAppSpeechRecognizeSession.cpp
./ur_mrcp_client/src/MrcpCliAppSynthScenario.cpp
./ur_mrcp_client/src/MrcpCliAppSynthSession.cpp
./ur_mrcp_client/src/UniMrcpCliAppFw.cpp
./ur_mrcp_client/src/UniMrcpCliAppFwSpeechRecog.cpp
./ur_mrcp_client/src/WrapMrcpSession.cpp
./ur_mrcp_client/src/WrapSpeechRecogMrcpSession.cpp
./ur_mrcp_client/src/xGateMrcpCliAppHandler.cpp
./ur_mrcp_client/src/xGateMrcpCliAppService.cpp
./ur_mrcp_client/src/URMrcpSessionHandler.cpp
./xg_schedulerservice/xGateSchedulerService.cpp
./xg_schedulerservice/xGateSchedulerHandler.cpp
./xg_schedulerservice/SchedulerContext.cpp
./src/URSpeechmaticssttclient.cpp
./src/URInhousesttclient.cpp
./ur_mrcp_client/src/xGateInhouseTtsClient.cpp)

target_link_libraries(${NAME} ${GSTREAMER_LIBRARIES} ${GLIB_LIBRARIES} ${GIO_LIBRARIES} gio-2.0 gobject-2.0 glib-2.0 ${LIBS} gstreamer-1.0 gstrtp-1.0 gstbase-1.0 gstapp-1.0)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/mrcp.conf DESTINATION /etc/xgate)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/start_stt_tts_mg DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/build)
