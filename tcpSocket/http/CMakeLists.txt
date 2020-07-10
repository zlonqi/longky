FIND_LIBRARY(BASE base HINTS ${PROJECT_SOURCE_DIR}/lib)
FIND_LIBRARY(NET net HINTS ${PROJECT_SOURCE_DIR}/lib)
FIND_LIBRARY(HTTP http HINTS ${PROJECT_SOURCE_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR}/threading/ ${PROJECT_SOURCE_DIR}/tcpSocket/ ./)
aux_source_directory(./ SRC)
add_library(http HttpServer.cpp ${SRC})
add_executable(http_test ./tests/HttpServer_test.cc)
target_link_libraries(http_test ${NET} ${BASE} ${HTTP})
