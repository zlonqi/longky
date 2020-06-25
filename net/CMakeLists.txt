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
FIND_LIBRARY(BASE muduo_base HINTS ${PROJECT_SOURCE_DIR}/lib)
add_library(muduo_net ${net_SRCS})
target_link_libraries(muduo_net ${BASE})

#add_library(muduo_net_cpp11 ${net_SRCS})
#target_link_libraries(muduo_net_cpp11 muduo_base_cpp11)
#set_target_properties(muduo_net_cpp11 PROPERTIES COMPILE_FLAGS "-std=c++0x")

#install(TARGETS muduo_net DESTINATION lib)
#install(TARGETS muduo_net_cpp11 DESTINATION lib)



