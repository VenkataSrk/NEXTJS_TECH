cmake_minimum_required(VERSION 2.8.12.2 FATAL_ERROR) 

set(NAME vid_rec)
project(${NAME})

FIND_PACKAGE(PkgConfig)
PKG_CHECK_MODULES(GLIB REQUIRED glib-2.0>=2.66.3)
PKG_CHECK_MODULES(GSTREAMER REQUIRED gstreamer-1.0>=1.18.2)
PKG_CHECK_MODULES(GSTREAMER_SDP REQUIRED gstreamer-sdp-1.0)
PKG_CHECK_MODULES(GSTREAMER_WEBRTC REQUIRED gstreamer-webrtc-1.0)
PKG_CHECK_MODULES(OPENSSL REQUIRED openssl>=1.1.1g)
PKG_CHECK_MODULES(GSTREAMER_GIO REQUIRED gio-2.0)
PKG_CHECK_MODULES(GSTREAMER_GOBJECT REQUIRED gobject-2.0)
PKG_CHECK_MODULES(GSTREAMER_GMODULE REQUIRED gmodule-2.0)
PKG_CHECK_MODULES(GSTREAMER_APP REQUIRED gstreamer-app-1.0)
PKG_CHECK_MODULES(GSTREAMER_RTP REQUIRED gstreamer-rtp-1.0)


set(CMAKE_CXX_FLAGS "-std=c++17 -lstdc++fs -g -fpermissive -z muldefs -DH248_MG -DXGATE_HAS_SELF_AUDIT")
#set(CMAKE_CXX_FLAGS "-std=c++0x -g -fpermissive -z muldefs -DH248_MG -DXGATE_HAS_SELF_AUDIT")
#set(CMAKE_CXX_FLAGS "-std=c++0x -Wall -Wextra")
#set(CMAKE_CXX_FLAGS "-std=c++11")


IF(NOT APPLE AND NOT MSVC)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
ENDIF()

set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include_directories(xg_redisservice)
include_directories(rapidjson)
include_directories(xg_logger)
include_directories(xg_reactor)
include_directories(xg_iurmodule)
include_directories(xg_httpservice)
include_directories(xg_mediaservice)
#include_directories(xg_controller)
include_directories(sfu_controller)
#include_directories(mbc_controller)
include_directories(/usr/local/)
include_directories(/usr/local/ssl/include)
#include_directories(AudioCodes.v46)
#include_directories(slsip)
#include_directories(Aculab)
#include_directories(pcre-3.4)
#include_directories(SPT)
include_directories(xg_sfu)
include_directories(xg_dbservice)
include_directories(xg_sfu/include)
#include_directories(xg_mbc)
#include_directories(xg_mbc/include)
include_directories(inc)

add_subdirectory(xg_redisservice)
add_subdirectory(xg_logger)
add_subdirectory(xg_httpservice)
add_subdirectory(xg_mediaservice)
#add_subdirectory(AudioCodes.v46)
#add_subdirectory(slsip)
#add_subdirectory(Aculab)
#add_subdirectory(pcre-3.4)
#add_subdirectory(SPT)
add_subdirectory(xg_sfu)
#add_subdirectory(xg_mbc)
#add_subdirectory(xg_controller)
add_subdirectory(sfu_controller)
add_subdirectory(xg_dbservice)
#add_subdirectory(mbc_controller)
add_executable(${NAME}
src/main.cpp
src/XGConfig.cpp
src/xGateUtil.cpp
src/xGateMultiSessionTracker.cpp
src/xGateProcessor.cpp
xg_sfu/include/xGateHmpGstBin.h
xg_reactor/xGateScheduler.cpp
sfu_controller/XG_SFUController.cpp
#xg_controller/XGController.cpp
xg_iurmodule/IURModuleCallBack.cpp
xg_iurmodule/IURModuleConfig.cpp
xg_iurmodule/IURModule.cpp
xg_iurmodule/IURModuleMsg.cpp)

target_link_libraries(${NAME} stdc++fs)
target_link_libraries(${NAME} xglog)
target_link_libraries(${NAME} xghttpservice)
target_link_libraries(${NAME} xgmediaservice)
target_link_libraries(${NAME} xgredisservice)
target_link_libraries(${NAME} sfu_controller)
#target_link_libraries(${NAME} mbc_controller)
#target_link_libraries(${NAME} xgmbc)
target_link_libraries(${NAME} xgsfu)
target_link_libraries(${NAME} xg_dbservice)
target_link_libraries(${NAME} -lACE -lcurl -lpthread)
target_link_libraries(${NAME} ${GLIB_LDFLAGS})
target_link_libraries(${NAME} ${GSTREAMER_LDFLAGS})
target_link_libraries(${NAME} ${GSTREAMER_SDP_LDFLAGS})
target_link_libraries(${NAME} ${GSTREAMER_WEBRTC_LDFLAGS})
target_link_libraries(${NAME} ${OPENSSL_LDFLAGS})
target_link_libraries(${NAME} ${GSTREAMER_GIO_LDFLAGS})
target_link_libraries(${NAME} ${GSTREAMER_GOBJECT_LDFLAGS})
target_link_libraries(${NAME} ${GSTREAMER_GMODULE_LDFLAGS})
target_link_libraries(${NAME} ${GSTREAMER_APP_LDFLAGS})
target_link_libraries(${NAME} ${GSTREAMER_RTP_LDFLAGS})
# Link stdc++fs only if using an older GCC version
if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
    target_link_libraries(${NAME} stdc++fs)
endif()
