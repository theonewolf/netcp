/*****************************************************************************
*                                                                            *
* netcp.c - this file implements a program that reads a file and writes its  *
*           data to a socket                                                 *
*                                                                            *
* The MIT License (MIT)                                                      *
*                                                                            *
* Copyright (c) 2014 Wolfgang Richter <wolfgang.richter@gmail.com>           *
*                                                                            *
* Permission is hereby granted, free of charge, to any person obtaining a    *
* copy of this software and associated documentation files (the "Software"), *
* to deal in the Software without restriction, including without limitation  *
* the rights to use, copy, modify, merge, publish, distribute, sublicense,   *
* and/or sell copies of the Software, and to permit persons to whom the      *
* Software is furnished to do so, subject to the following conditions:       *
*                                                                            *
* The above copyright notice and this permission notice shall be included in *
* all copies or substantial portions of the Software.                        *
*                                                                            *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR *
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   *
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE*
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER     *
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    *
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        *
* DEALINGS IN THE SOFTWARE.                                                  *
******************************************************************************/

#define _LARGEFILE64_SOURCE
#include "netcp.h"



#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>



#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>



/* Utility functions */
bool checkaddr(char* ip, char* port, struct addrinfo** results)
{
    struct addrinfo hints;
    struct protoent* protocol;
    int res;

    if ((protocol = getprotobyname(PROTO)) == NULL)
    {
        fprintf(stderr, "Error: could not lookup protocol %s\n", PROTO);
        return true;
    }

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family     = AF_UNSPEC;
    hints.ai_socktype   = SOCK_STREAM;
    hints.ai_protocol   = protocol->p_proto;
    hints.ai_flags      = 0;
    endprotoent();

    if ((res = getaddrinfo(ip, port, &hints, results)))
    {
        fprintf(stderr, "Error: %s\n", gai_strerror(res));
        if (*results)
            freeaddrinfo(*results);
        return true;
    }

    return false;
}

bool checksock(int* sockfd, struct addrinfo* results)
{
    struct addrinfo* addr;

    for (addr = results; addr != NULL; addr = addr->ai_next)
    {
        if ((*sockfd = socket(addr->ai_family, addr->ai_socktype,
                              addr->ai_protocol)) < 0)
        {
            fprintf(stderr, "Error: %s\n", strerror(errno));
            continue;
        }

        if (connect(*sockfd, addr->ai_addr, addr->ai_addrlen) != -1)
        {
            break;
        }
        else
        {
            fprintf(stderr, "Error: %s\n", strerror(errno));
        }

        if (*sockfd && (close(*sockfd) < 0))
        {
                fprintf(stderr, "Error: %s\n", strerror(errno));
                return true;
        }
    }

    if (addr == NULL)
    {
        fprintf(stderr, "Error: could not find a working address.\n");
        return true;
    }
    else
    {
        return false;
    }
}

bool checkfile(int* fd, char* fname)
{
    if ((*fd = open(fname, O_RDONLY | O_LARGEFILE)) < 0)
    {
        fprintf(stderr, "Error: %s\n", strerror(errno));
        return true;
    }

    return false;
}

bool copyfile(int fd, int sockfd)
{
    uint8_t buf[BUF_SIZE];
    ssize_t readc = 0, wrotec = 0, pos = 0;

    while ((readc = read(fd, buf, BUF_SIZE)) > 0)
    {
        pos = 0;

        while (readc > 0 &&
               ((wrotec = write(sockfd, &(buf[pos]), readc - pos)) > 0))
        {
            pos += wrotec;
        }

        if (wrotec < 0)
        {
            fprintf(stderr, "Error: %s\n", strerror(errno));
            return true;
        }
    } 

    if (readc < 0)
    {
        fprintf(stderr, "Error: %s\n", strerror(errno));
        return true;
    }

    return false;
}

/* Main logic using utility functions */
int main(int argc, char* argv[])
{
    char* fname = NULL, *ip = NULL, *port = NULL;
    struct addrinfo* results = NULL;
    int fd = 0, sockfd = 0, ret = EXIT_SUCCESS;

    fprintf(stdout, BANNER);

    if (argc < 4)
    {
        fprintf(stdout, USAGE, argv[0]);
        return EXIT_FAILURE;
    }

    fname = argv[1];
    ip = argv[2];
    port = argv[3];

    fprintf(stdout, INFO, fname, ip, port);

    if (checkaddr(ip, port, &results))
    {
        ret = EXIT_FAILURE;
        goto exit;
    }

    if (checksock(&sockfd, results))
    {
        ret = EXIT_FAILURE;
        goto exit;
    }

    if (checkfile(&fd, fname))
    {
        ret = EXIT_FAILURE;
        goto exit;
    }

    if (copyfile(fd, sockfd))
    {
        ret = EXIT_FAILURE;
        goto exit;
    }

exit:
    if (results)
        freeaddrinfo(results);

    if (sockfd >= 0)
        close(sockfd);

    if (fd >= 0)
        close(fd);

    return ret;
}
