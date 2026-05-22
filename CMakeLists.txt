cmake_minimum_required(VERSION 3.11 FATAL_ERROR)

set(NAME mvno_service)
project(${NAME})

set(CMAKE_CXX_FLAGS "-std=c++1z -lrt -lm")
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG -g3")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
#set(EXECUTABLE_OUTPUT_PATH ../)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/includes)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/includes/CAInternal)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/Logger)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/utilities)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/utilities/json_parser)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/utilities/xml_parser)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/report)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/DB)
include_directories(/usr/include)
include_directories(/usr/local/include)
include_directories(/usr/include/mysql-cppconn-8/jdbc)

link_directories(/usr/lib)
link_directories(/usr/lib64)
link_directories(/usr/local/lib)
link_directories(/usr/local/lib64)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/utilities) 

add_executable(${NAME}
src/main.cpp
src/MVNOBusinessService.cpp
src/BusinessServer.cpp
src/BusinessConfigService.cpp
src/BusinessConfigHandler.cpp
src/BSRequestProcessor.cpp
src/BusinessConfigObject.cpp
src/RoutingService.cpp
src/BSCallObject.cpp
src/BSDBService.cpp
src/BSInterface.cpp
src/BSOperations.cpp
src/BSCharging.cpp
src/BSRouting.cpp
src/BSSwitching.cpp
src/BSGmsc.cpp
src/BSException.cpp
src/report/BSBaseReport.cpp
src/report/ReportingService.cpp
src/report/BSCDR.cpp
src/report/BSCDS.cpp
src/report/BSCRR.cpp
src/report/BSDAT.cpp
src/DB/BSLocalDBHandler.cpp
src/DB/BSLocalDBService.cpp
src/BSOperations.cpp
src/BSHttpService.cpp)

target_link_libraries(${NAME} -lACE -lssl -lcrypto -lz -lpthread -lodbc -lcurl -lmysqlcppconn -lodbc) 
target_link_libraries(${NAME} cautil)

install(CODE
"IF(EXISTS \"/etc/systemd/system/mvno.service\")
  message(STATUS \"Service file(mvno.service) - found\")
ELSE()
  message(AUTHOR_WARNING \"Service file(mvno.service) - not found\")
  message(STATUS \"Service file(mvno.service) - copying ...\")
  file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/mvno.service DESTINATION /lib/systemd/system)
  file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/mvno.service DESTINATION /etc/systemd/system)
  message(STATUS \"Service file(mvno.service) - copied\")
ENDIF()")

install(TARGETS ${NAME} DESTINATION /usr/bin)
# Create config directory
install(CODE "execute_process(COMMAND mkdir -p /etc/xgate)")
install(CODE "execute_process(COMMAND mkdir -p /var/log/xgate/mvno)")
install(CODE "execute_process(COMMAND systemctl daemon-reload)")
install(CODE "execute_process(COMMAND systemctl enable mvno.service)")
install(CODE "execute_process(COMMAND systemctl restart mvno.service)
message(STATUS \"Service file(mvno.service) ==> STARTED\")")

set_property(TARGET ${NAME} APPEND PROPERTY ADDITIONAL_CLEAN_FILES /usr/bin/mvno
/var/log/xgate/mvno/mvno_service.out /var/log/xgate/mvno/mvno_service.err)


#set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES)
