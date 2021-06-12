include_directories(
        ${PROJECT_SOURCE_DIR}/base
        ${PROJECT_SOURCE_DIR}/base/thread
        ${PROJECT_SOURCE_DIR}/netLayer
        ${CMAKE_CURRENT_SOURCE_DIR})

FIND_LIBRARY(BASE base HINTS ${PROJECT_SOURCE_DIR}/lib)
FIND_LIBRARY(NET net HINTS ${PROJECT_SOURCE_DIR}/lib)
FIND_LIBRARY(YAML yaml-cpp HINTS /usr/lib)#must do like this, or it can't find libyaml-cpp.so
add_executable(fastcgi cgiserver.cpp fastcgi.cpp parseRequest.cpp configLoad.cpp start_up.cpp)
target_link_libraries(fastcgi ${BASE} ${NET} ${YAML})
