INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/event
        ${CMAKE_CURRENT_SOURCE_DIR}/http
        ${CMAKE_CURRENT_SOURCE_DIR}/poller
        ${CMAKE_CURRENT_SOURCE_DIR}/socket
        ${CMAKE_CURRENT_SOURCE_DIR}/tcp
        ${CMAKE_CURRENT_SOURCE_DIR}/timer
        ${CMAKE_CURRENT_SOURCE_DIR})
#SET(LIBRARY_OUTPUT_PATH  ${CMAKE_CURRENT_SOURCE_DIR}/../lib)
SET(net_SRCS
        tcp/s/Acceptor.cpp
        Buffer.cpp
        event/Channel.cpp
        tcp/c/Connector.cpp
        event/eventloop/EventLoop.cpp
        event/eventloop/EventLoopThread.cpp
        event/eventloop/EventLoopThreadPool.cpp
        socket/InetAddress.cpp
        poller/Poller.cpp
        poller/DefaultPoller.cpp
        poller/EpollPoller.cpp
        poller/PollPoller.cpp
        socket/Socket.cpp
        socket/SocketsOps.cpp
        tcp/c/client/TcpClient.cpp
        tcp/TcpConnection.cpp
        tcp/s/server/TcpServer.cpp
        timer/Timer.cpp
        timer/TimerQueue.cpp
        )
FIND_LIBRARY(BASE base HINTS ${PROJECT_SOURCE_DIR}/lib)
add_library(net ${net_SRCS})
target_link_libraries(net ${BASE})





