FIND_LIBRARY(BASE base HINTS ${PROJECT_SOURCE_DIR}/lib)
FIND_LIBRARY(NET net HINTS ${PROJECT_SOURCE_DIR}/lib)
FIND_LIBRARY(HTTP http HINTS ${PROJECT_SOURCE_DIR}/lib)

include_directories(
    ${PROJECT_SOURCE_DIR}/base/ 
    ${PROJECT_SOURCE_DIR}/base/log/
    ${PROJECT_SOURCE_DIR}/base/time/ 
    ${PROJECT_SOURCE_DIR}/base/thread/ 
    ${PROJECT_SOURCE_DIR}/tcpSocket/ 
    ${PROJECT_SOURCE_DIR}/tcpSocket/http/ 
    ./)
add_executable(procmon procmon.cc plot.cc)
target_link_libraries(procmon ${HTTP} ${NET} ${BASE} gd)


