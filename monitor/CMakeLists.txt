include_directories(../base/log/ ../base/ ../tcpSocket/ .)

FIND_LIBRARY(BASE base HINTS ${PROJECT_SOURCE_DIR}/lib)
FIND_LIBRARY(NET net HINTS ${PROJECT_SOURCE_DIR}/lib)
add_executable(HeartBeatChecker heartBeatMonitor.cpp)
target_link_libraries(HeartBeatChecker ${BASE} ${NET})
