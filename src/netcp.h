#ifndef __NETCP_
#define __NETCP_

#define VERSION_MAJOR 1
#define VERSION_MINOR 1

#define BANNER      "-- netcp v%d.%d by Wolfgang Richter " \
    "<wolfgang.richter@gmail.com> --\n"
#define USAGE       "%s <filename> <IP> <port>\n"
#define INFO        "copying '%s' to %s:%s\n"

#define PROTO       "TCP"
#define BUF_SIZE    4096

#define CHECK(X) \
if (X) \
{ \
    ret = EXIT_FAILURE; \
    goto exit; \
}

#define CHECK_SYSCALL(X) \
if (X < 0) \
{ \
    fprintf(stderr, "Error: %s\n", strerror(errno)); \
    return true; \
}

#endif
