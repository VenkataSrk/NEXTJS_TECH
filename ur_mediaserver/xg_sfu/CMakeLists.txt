cmake_minimum_required(VERSION 2.8.12.2) 

link_directories($(ACE_ROOT)/ace)
link_directories(/usr/local/lib)
link_directories(/usr/local/lib/x86_64-linux-gnu)
link_directories(/usr/lib64)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g  -DNDEBUG -w -fpermissive -z muldefs `pkg-config --cflags --libs gstreamer-1.0 gstreamer-sdp-1.0 gstreamer-webrtc-1.0` `pkg-config --cflags --libs glib-2.0` -DH248_MG -DGST_WEBRTC -DHAVE_GST_DEBUG_PRINTS -DXGATE_HAS_GSTREAMER -DUSE_GST_AUDIO_MIXER_PLUGIN")

include_directories("include")

add_library(xgsfu
src/xGateHmpGstUtil.cpp
src/xGateSFUtil.cpp
src/xGateHmpGstVideoBin.cpp
src/xGateHmpGstBin.cpp
src/SFUBridgeProcessor.cpp
src/SFUClient.cpp
src/xGateSFUService.cpp
src/SFUController.cpp
src/xGateHmpGstRateControl.cpp
src/URInhousesttclient.cpp
src/xGateHmpGstMRCPVoice.cpp
src/xGateHmpVP9Parser.cpp
src/xGateHmpGstActivSpk.cpp
src/xGateHmpThreadPool.cpp
src/SFUClientMediaHandler.cpp)

string (FIND ${CMAKE_CXX_FLAGS} USE_GST_AUDIO_MIXER_PLUGIN var_found)
if (var_found LESS 0 )
 add_library (xgmgmixer src/xGateHmpGstAudioMixer.cpp)
endif()

target_link_libraries (xgsfu -lACE -luuid )

if (var_found LESS 0 )
target_link_libraries (xgsfu xgmgmixer)
endif()
#target_link_libraries(xgsfu -lACE -luuid ) #-lefence)
