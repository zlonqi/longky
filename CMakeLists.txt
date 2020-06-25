cmake_minimum_required(VERSION 3.9.1)
project(muduo)
#
set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_FLAGS "-pthread -fsanitize=address -g")
set(CMAKE_CXX_FLAGS "-pthread")
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY  ${PROJECT_SOURCE_DIR}/lib)

add_subdirectory(base)
add_subdirectory(net)
#add_subdirectory(./examples/procmon)
#add_subdirectory(fastcgi)
add_subdirectory(staticPage)
add_subdirectory(heartBeatMonitor)

