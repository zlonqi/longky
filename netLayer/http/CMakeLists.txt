FIND_LIBRARY(BASE base HINTS ${PROJECT_SOURCE_DIR}/lib)
FIND_LIBRARY(NET net HINTS ${PROJECT_SOURCE_DIR}/lib)
FIND_LIBRARY(HTTP http HINTS ${PROJECT_SOURCE_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR}/base/ ${PROJECT_SOURCE_DIR}/netLayer/ ./)
aux_source_directory(./ SRC)
add_library(http HttpServer.cpp ${SRC})
