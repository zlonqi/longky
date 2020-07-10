include_directories(../base/ ../tcpSocket/ .)

FIND_LIBRARY(BASE base HINTS ${PROJECT_SOURCE_DIR}/lib)
FIND_LIBRARY(NET net HINTS ${PROJECT_SOURCE_DIR}/lib)
add_executable(fstcgi cgiserver.cpp fastcgi.cpp parseRequest.cpp)
target_link_libraries(fstcgi ${BASE} ${NET} yaml-cpp)
