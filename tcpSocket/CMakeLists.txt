INCLUDE_DIRECTORIES(./ ./poller/)
#SET(LIBRARY_OUTPUT_PATH  ${CMAKE_CURRENT_SOURCE_DIR}/../lib)
set(net_SRCS
        Acceptor.cpp
        Buffer.cpp
        Channel.cpp
        Connector.cpp
        EventLoop.cpp
        EventLoopThread.cpp
        EventLoopThreadPool.cpp
        InetAddress.cpp
        Poller.cpp
        poller/DefaultPoller.cpp
        poller/EpollPoller.cpp
        poller/PollPoller.cpp
        Socket.cpp
        SocketsOps.cpp
        TcpClient.cpp
        TcpConnection.cpp
        TcpServer.cpp
        Timer.cpp
        TimerQueue.cpp
        )
FIND_LIBRARY(BASE base HINTS ${PROJECT_SOURCE_DIR}/lib)
add_library(net ${net_SRCS})
target_link_libraries(net ${BASE})





