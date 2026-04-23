#ifndef SYS_SOCKET_H
#define SYS_SOCKET_H

#include <sys/types.h>

// Protocol families
#define PF_UNSPEC       0   // Unspecified
#define PF_UNIX         1   // Local Unix
#define PF_LOCAL        PF_UNIX
#define PF_INET         2   // Internet
#define PF_INET6        10  // IPv6

// Address families
#define AF_UNSPEC       PF_UNSPEC
#define AF_UNIX         PF_UNIX
#define AF_LOCAL        PF_LOCAL
#define AF_INET         PF_INET
#define AF_INET6        PF_INET6

// Socket types
#define SOCK_STREAM     1   // Stream socket
#define SOCK_DGRAM      2   // Datagram socket
#define SOCK_RAW        3   // Raw protocol interface
#define SOCK_RDM        4   // Reliably delivered messages
#define SOCK_SEQPACKET  5   // Sequenced packet stream

// Socket flags
#define SOCK_NONBLOCK   0x00000001  // Non-blocking
#define SOCK_CLOEXEC    0x00000002  // Close on exec

// Protocol families for socket level
#define SOL_SOCKET      1   // Options for socket level

// Socket options
#define SO_DEBUG        1   // Debug information
#define SO_REUSEADDR    2   // Allow local address reuse
#define SO_TYPE         3   // Get socket type
#define SO_ERROR        4   // Get and clear error status
#define SO_DONTROUTE    5   // Don't use routing
#define SO_BROADCAST    6   // Permit broadcasting
#define SO_SNDBUF       7   // Send buffer size
#define SO_RCVBUF       8   // Receive buffer size
#define SO_KEEPALIVE    9   // Keep connections alive
#define SO_OOBINLINE    10  // Leave out-of-band data inline
#define SO_NO_CHECK     11  // Disable checksum
#define SO_PRIORITY     12  // Priority
#define SO_LINGER       13  // Linger on close if data present
#define SO_BSDCOMPAT    14  // BSD compatibility
#define SO_REUSEPORT    15  // Allow local address/port reuse
#define SO_PASSCRED     16  // Pass credentials
#define SO_PEERCRED     17  // Get peer credentials
#define SO_RCVLOWAT     18  // Receive low water mark
#define SO_SNDLOWAT     19  // Send low water mark
#define SO_RCVTIMEO     20  // Receive timeout
#define SO_SNDTIMEO     21  // Send timeout

// Socket address structure
struct sockaddr {
    sa_family_t sa_family;  // Address family
    char sa_data[14];       // Address data
};

// Unix domain socket address
struct sockaddr_un {
    sa_family_t sun_family; // AF_UNIX
    char sun_path[108];     // Pathname
};

// Internet address structure
struct sockaddr_in {
    sa_family_t sin_family; // AF_INET
    uint16_t sin_port;      // Port number
    struct in_addr sin_addr; // Internet address
    unsigned char sin_zero[8]; // Padding
};

// IPv6 address structure
struct sockaddr_in6 {
    sa_family_t sin6_family; // AF_INET6
    uint16_t sin6_port;     // Port number
    uint32_t sin6_flowinfo;  // Flow information
    struct in6_addr sin6_addr; // IPv6 address
    uint32_t sin6_scope_id;  // Scope ID
};

// Internet address
struct in_addr {
    uint32_t s_addr;        // IPv4 address
};

// IPv6 address
struct in6_addr {
    unsigned char s6_addr[16]; // IPv6 address
};

// Socket functions
int socket(int domain, int type, int protocol);
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int listen(int sockfd, int backlog);
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
ssize_t send(int sockfd, const void *buf, size_t len, int flags);
ssize_t recv(int sockfd, void *buf, size_t len, int flags);
ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
               const struct sockaddr *dest_addr, socklen_t addrlen);
ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
                 struct sockaddr *src_addr, socklen_t *addrlen);
int shutdown(int sockfd, int how);
int getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen);
int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);

// Shutdown flags
#define SHUT_RD         0   // No more receptions
#define SHUT_WR         1   // No more transmissions
#define SHUT_RDWR       2   // No more receptions or transmissions

// Message flags
#define MSG_OOB         0x0001  // Process out-of-band data
#define MSG_PEEK        0x0002  // Peek at incoming messages
#define MSG_DONTROUTE   0x0004  // Don't use routing
#define MSG_TRYHARD     0x0004  // Compatibility
#define MSG_CTRUNC      0x0008  // Control data lost
#define MSG_PROBE       0x0010  // Status probe
#define MSG_TRUNC       0x0020  // Data truncated
#define MSG_DONTWAIT    0x0040  // Non-blocking I/O
#define MSG_EOR         0x0080  // End of record
#define MSG_WAITALL     0x0100  // Wait for full request
#define MSG_FIN         0x0200  // Finish
#define MSG_SYN         0x0400  // Synchronize
#define MSG_CONFIRM     0x0800  // Confirm path validity
#define MSG_RST         0x1000  // Reset
#define MSG_ERRQUEUE    0x2000  // Fetch error from queue
#define MSG_NOSIGNAL    0x4000  // Don't raise SIGPIPE
#define MSG_MORE        0x8000  // More data to send

#endif // SYS_SOCKET_H
