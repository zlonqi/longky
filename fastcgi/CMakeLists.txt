include_directories(../base/ ../net/ .)

FIND_LIBRARY(BASE muduo_base HINTS ${PROJECT_SOURCE_DIR}/lib)
FIND_LIBRARY(NET muduo_net HINTS ${PROJECT_SOURCE_DIR}/lib)
add_executable(fstcgi cgiserver.cpp fastcgi.cpp parseRequest.cpp)
target_link_libraries(fstcgi ${BASE} ${NET} yaml-cpp)