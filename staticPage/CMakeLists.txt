include_directories(../base/log/ ../base/thread/ ../base/time/ ../base/ ../net/ ./)

FIND_LIBRARY(BASE muduo_base HINTS ${PROJECT_SOURCE_DIR}/lib)
FIND_LIBRARY(NET muduo_net HINTS ${PROJECT_SOURCE_DIR}/lib)
add_executable(webServer staticServer.cpp requestParser.cpp start_up.cpp ZLibString.cpp
        configLoad.cpp redis.cpp)
target_link_libraries(webServer ${BASE} ${NET} yaml-cpp crypto z hiredis)
