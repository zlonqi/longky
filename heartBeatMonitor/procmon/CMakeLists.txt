FIND_LIBRARY(BASE muduo_base HINTS ${PROJECT_SOURCE_DIR}/lib)
FIND_LIBRARY(NET muduo_net HINTS ${PROJECT_SOURCE_DIR}/lib)
FIND_LIBRARY(HTTP muduo_http HINTS ${PROJECT_SOURCE_DIR}/lib)

include_directories(
    ${PROJECT_SOURCE_DIR}/base/ 
    ${PROJECT_SOURCE_DIR}/base/log/
    ${PROJECT_SOURCE_DIR}/base/time/ 
    ${PROJECT_SOURCE_DIR}/base/thread/ 
    ${PROJECT_SOURCE_DIR}/net/ 
    ${PROJECT_SOURCE_DIR}/net/http/ 
    ./)
add_executable(procmon procmon.cc plot.cc)
target_link_libraries(procmon ${HTTP} ${NET} ${BASE} gd)


