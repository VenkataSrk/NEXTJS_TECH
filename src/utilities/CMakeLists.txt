cmake_minimum_required(VERSION 3.11 FATAL_ERROR)

# Output directory
set(LIBRARY_OUTPUT_PATH /usr/local/lib)

# Source files
add_library(cautil
    CAThreadPoolManager.cpp
    CAThread.cpp
    CAString.cpp
    CASocket.cpp
    CASocketContext.cpp
    CASocketReceiver.cpp
    CAHash.cpp
    CAFile.cpp
    CASTLBase.cpp
    json_parser/RapidJSON.cpp
    xml_parser/RapidXML.cpp
    CAClientSocket.cpp
    CAServerSocket.cpp
    ../Logger/LogProfile.cpp
    ../Logger/logger.cpp
    ../Logger/LogHandler.cpp
    CADBDriver.cpp
    CAFileNotifier.cpp
)

# Link libraries
target_link_libraries(cautil -lACE -lpthread -lodbc)

