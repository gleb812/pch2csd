#include "string.h"
#include "stdlib.h"
#include "stdio.h"

#ifndef pch2csd_Util_h
#define pch2csd_Util_h


inline char *PreparePathString(const char *pathString) {
#ifdef __WIN32__
    const char *from = "/";
    const char *to = "\\";
#else
    char from = '\\';
    char to = '/';
#endif
    char *newString = (char *) malloc(sizeof(char) * strlen(pathString));
    strcpy(newString, pathString);

    char *p = newString;
    while (*p != '\0') {
        if (*p == from) {
            *p = to;
        }
        p++;
    }
    return newString;
}

#endif /* pch2csd_Util_h */

