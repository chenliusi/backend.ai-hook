#include "utils.hh"
#include "patch-libs.hh"

#include <cstdarg>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cassert>
#include <cerrno>

#include <ctype.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

static const char *input_host = "127.0.0.1";
static const int input_port = 65000;


OVERRIDE_LIBC_SYMBOL(long, sysconf, int flag)
    switch (flag) {
    case _SC_NPROCESSORS_ONLN:
    case _SC_NPROCESSORS_CONF:
        return get_num_processors_from_cpuset("/sys/fs/cgroup/cpuset/cpuset.cpus");
    default:
        break;
    }
    return orig_sysconf(flag);
}


extern "C"
int scanf(const char *format, ...)
{
    va_list args;
    char buffer[1024];
    struct sockaddr_in addr;
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -errno;
    }

    fflush(stdout);

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(input_host);
    addr.sin_port = htons(input_port);

    if (connect(sockfd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        perror("connect");
        return -errno;
    }

    int recvsz = read(sockfd, buffer, 1023);
    close(sockfd);
    buffer[recvsz] = '\0';

    va_start(args, format);
    int ret = vsscanf(buffer, format, args);
    va_end(args);
    return ret;
}


extern "C"
int vscanf(const char *format, va_list args)
{
    char buffer[1024];
    struct sockaddr_in addr;
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -errno;
    }

    fflush(stdout);

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(input_host);
    addr.sin_port = htons(input_port);

    if (connect(sockfd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        perror("connect");
        return -errno;
    }

    int recvsz = read(sockfd, buffer, 1023);
    close(sockfd);
    buffer[recvsz] = '\0';

    int ret = vsscanf(buffer, format, args);
    return ret;
}
