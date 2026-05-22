cmake_minimum_required(VERSION 2.8.12.2) 

link_directories($(ACE_ROOT)/ace)
link_directories(/usr/local/lib)
link_directories(/usr/local/lib/x86_64-linux-gnu)
link_directories(/usr/lib64)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g  -DNDEBUG -w -fpermissive -z muldefs `pkg-config --cflags --libs gstreamer-1.0 gstreamer-sdp-1.0 gstreamer-webrtc-1.0` `pkg-config --cflags --libs glib-2.0` -DH248_MG -DGST_WEBRTC -DHAVE_GST_DEBUG_PRINTS -DXGATE_HAS_GSTREAMER")

include_directories("include")

add_library(xgsfu
src/xGateHmpGstUtil.cpp
src/xGateHmpGstAudioBin.cpp
src/xGateSFUtil.cpp
src/xGateHmpGstBin.cpp
src/xGateHmpGstChannel.cpp
src/SFUBridgeProcessor.cpp
src/SFUClient.cpp
src/xGateSFUService.cpp
src/SFUController.cpp)
       
target_link_libraries(xgsfu -lACE -luuid -lavformat -lavutil -lavcodec)

