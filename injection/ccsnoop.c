/* file: ccsnoop.c
 * 
 * author: lightbulbone
 * description: log communication between CommCenter and whatever it talks to
 *
 * compilation: 
 * 1. Set environment variable ARM_CC=<path-to-arm-compiler>
 * 2. Set environment variable SYSROOT=<iphone-sdk>
 * 3. Compile 
 * 
 * For example:
 *   bash $ export ARM_CC=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/arm-apple-darwin10-llvm-gcc-4.2
 *   bash $ export SYSROOT=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS6.1.sdk/
 *   bash $ ${ARM_CC} -Wall -dynamiclib -isysroot ${SYSROOT} -o ccsnoop.dylib ccsnoop.c
 *
 */

/* Copyright (c) 2013, LightBulbOne
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the LightBulbOne nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdarg.h> 
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syslimits.h>

const char *logPath = "/var/wireless/Library/Logs/ccsnoop.log";
FILE *logFile = NULL;

typedef struct interposer_s {
    void* new_func;
    void* origin_func;
} interposer_t;

int ccs_open(const char *, int, mode_t);
int ccs_close(int);
int ccs_read(int, void *, int);
int ccs_write(int, void *, int);

static const interposer_t interposers[] __attribute__ ((section("__DATA, __interpose"))) =
{
    { (void *)ccs_open, (void *)open },
    { (void *)ccs_close, (void *)close},
    { (void *)ccs_read, (void *)read},
    { (void *)ccs_write, (void *)write},
};

void ccs_log(const char *fmt, ...)
{
    va_list fp;

    if(logFile == NULL) {
        logFile = fopen(logPath, "a*");
        if(!logFile)
            return;
    }

    va_start(fp, fmt);
    vfprintf(logFile, fmt, fp);

    fflush(logFile);
}

int ccs_open(const char *path, int flags, mode_t mode)
{
    ccs_log("open [%s]\n", path);

    int ret = open(path, flags, mode);
    return ret;
}

int ccs_close(int fd)
{
    char filePath[PATH_MAX];
    if (fcntl(fd, F_GETPATH, filePath) != -1)
    {
        ccs_log("close [%s]\n", filePath);
    }

    int ret = close(fd);
    return ret;
}

int ccs_read(int fd, void *buffer, int nbytes)
{
    int ret = read(fd, buffer, nbytes);

    char filePath[PATH_MAX];
    if (fcntl(fd, F_GETPATH, filePath) != -1)
    {
        ccs_log("read [%s] %d bytes => %s\n", filePath, nbytes, (char *)buffer);
    }
    else
    {
        ccs_log("read [UNKNOWN] %d bytes => %s\n", nbytes, (char *)buffer);
    }

    return ret;
}

int ccs_write(int fd, void *buffer, int nbyte)
{
    int ret = write(fd, buffer, nbyte);

    char filePath[PATH_MAX];
    if (fcntl(fd, F_GETPATH, filePath) != -1)
    {
        ccs_log("write [%s] %d bytes => %s\n", filePath, nbyte, (char *)buffer);
    }
    else
    {
        ccs_log("write [UNKNOWN] %d bytes => %s\n", nbyte, (char *)buffer);
    }

    return ret;
}
