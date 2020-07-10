cmake_minimum_required(VERSION 3.9.1)
project(longky)
#
set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_FLAGS "-pthread -fsanitize=address -g")
set(CMAKE_CXX_FLAGS "-pthread")
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY  ${PROJECT_SOURCE_DIR}/lib)

add_subdirectory(base)
add_subdirectory(tcpSocket)
add_subdirectory(./monitor/procmon)
add_subdirectory(cgiServer)
add_subdirectory(webServer)
add_subdirectory(monitor)

