//
// Created by root on 2019/12/21.
//

#ifndef SOCKET_H
#define SOCKET_H

#include "base/noncopyable.h"

// struct tcp_info is in <netinet/tcp.h>
struct tcp_info;

namespace tank
{
///
/// TCP networking.
///
    namespace net
    {

        class InetAddress;

///
/// Wrapper of socket file descriptor.
///
/// It closes the sockfd when desctructs.
/// It's thread safe, all operations are delagated to OS.
        class Socket : noncopyable
        {
        public:
            explicit Socket(int sockfd)
                    : sockfd_(sockfd),
                    setSolinger_(false)
            { }

            // Socket(Socket&&) // move constructor in C++11
            ~Socket();

            void setSolinger(){ setSolinger_=true;}
            int fd() const { return sockfd_; }
            // return true if success.
            bool getTcpInfo(struct tcp_info*) const;
            bool getTcpInfoString(char* buf, int len) const;

            /// abort if address in use
            void bindAddress(const InetAddress& localaddr);
            /// abort if address in use
            void listen();
            /// close fd_, send rst
            void close();
            /// On success, returns a non-negative integer that is
            /// a descriptor for the accepted socket, which has been
            /// set to non-blocking and close-on-exec. *peeraddr is assigned.
            /// On error, -1 is returned, and *peeraddr is untouched.
            int accept(InetAddress* peeraddr);

            void shutdownWrite();

            ///
            /// Enable/disable TCP_NODELAY (disable/enable Nagle's algorithm).
            ///
            void setTcpNoDelay(bool on);

            ///
            /// Enable/disable SO_REUSEADDR
            ///
            void setReuseAddr(bool on);

            ///
            /// Enable/disable SO_REUSEPORT
            ///
            void setReusePort(bool on);

            ///
            /// Enable/disable SO_KEEPALIVE
            ///
            void setKeepAlive(bool on);

        private:
            bool setSolinger_;
            const int sockfd_;
        };

    }  // namespace net
}  // namespace tank

#endif //SOCKET_H
