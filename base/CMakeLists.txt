cmake_minimum_required(VERSION 3.15)
project(longky)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-pthread")
SET(LIBRARY_OUTPUT_PATH  ${CMAKE_CURRENT_SOURCE_DIR}/../lib)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/log
    ${CMAKE_CURRENT_SOURCE_DIR}/time
    ${CMAKE_CURRENT_SOURCE_DIR}/thread
    ${CMAKE_CURRENT_SOURCE_DIR}/
)

set(base_SRCS
        thread/Condition.cpp
        thread/CountDownLatch.cpp
        thread/CurrentThread.cpp
        thread/Thread.cpp
        thread/ProcessInfo.cpp
        thread/ThreadPool.cpp
        log/AyncLogging.cpp
        log/LogFile.cpp
        log/FileUtil.cpp
        log/Logging.cpp
        log/LogStream.cpp
        time/Date.cpp
        time/Timestamp.cpp
        time/TimeZone.cpp
        Exception.cpp
        )

#add_executable(base ${base_SRCS})
add_library(base ${base_SRCS})
target_link_libraries(base pthread rt)

