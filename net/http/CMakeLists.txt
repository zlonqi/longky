FIND_LIBRARY(BASE muduo_base HINTS ${PROJECT_SOURCE_DIR}/lib)
FIND_LIBRARY(NET muduo_net HINTS ${PROJECT_SOURCE_DIR}/lib)
FIND_LIBRARY(HTTP muduo_http HINTS ${PROJECT_SOURCE_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR}/base/ ${PROJECT_SOURCE_DIR}/net/ ./)
aux_source_directory(./ SRC)
add_library(muduo_http HttpServer.cpp ${SRC})
add_executable(http_test ./tests/HttpServer_test.cc)
target_link_libraries(http_test ${NET} ${BASE} ${HTTP})
