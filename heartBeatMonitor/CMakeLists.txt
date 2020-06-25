include_directories(../base/log/ ../base/ ../net/ .)

FIND_LIBRARY(BASE muduo_base HINTS ${PROJECT_SOURCE_DIR}/lib)
FIND_LIBRARY(NET muduo_net HINTS ${PROJECT_SOURCE_DIR}/lib)
add_executable(HeartBeatChecker server.cpp)
target_link_libraries(HeartBeatChecker ${BASE} ${NET})
