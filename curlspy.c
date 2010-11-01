/*
 * Copyright © 2010 Jakub Wilk <jwilk@jwilk.net>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the “Software”), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#define _GNU_SOURCE

#include <assert.h>
#include <errno.h>
#include <dlfcn.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <curl/curl.h>

CURLcode Curl_setopt(CURL *handle, CURLoption option, va_list arg);

static int fd = STDOUT_FILENO;

void curlspy_init(void) __attribute__((constructor));

void curlspy_init(void)
{
    long longfd;
    char *endptr;
    const char *fdstring = getenv("CURLSPY_FD");
    if (fdstring == NULL)
        return;
    errno = 0;
    longfd = strtol(fdstring, &endptr, 10);
    if (*endptr != '\0' || errno != 0 || longfd < 0 || longfd >= INT_MAX)
    {
        errno = EBADF;
        perror("curlspy");
        longfd = STDOUT_FILENO;
        fprintf(stderr, "curlspy: Using fd=%ld instead\n", longfd);
    }
    fd = longfd;
}

#undef curl_easy_setopt
CURLcode curl_easy_setopt(CURL *handle, CURLoption option, ...)
{
    va_list args;
    CURLcode rc;

    if (handle == NULL)
        return CURLE_BAD_FUNCTION_ARGUMENT;

    va_start(args, option);
    if (option == CURLOPT_URL)
    {
        va_list args_copy;
        const char *parameter;
        int rc;
        va_copy(args_copy, args);
        parameter = va_arg(args_copy, const char *);
        rc = write(fd, parameter, strlen(parameter));
        if (rc >= 0)
            rc = write(fd, "\n", 1);
        if (rc < 0)
            perror("curlspy");
        va_end(args_copy);
    }
    rc = Curl_setopt(handle, option, args);
    va_end(args);
    return rc;
}

/* vim:set ts=4 sw=4 et:*/
