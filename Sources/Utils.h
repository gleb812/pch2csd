//
// Created by Ech on 4/11/17.
//

#ifndef PCH2CSD_UTILS_H
#define PCH2CSD_UTILS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct StringArray {
    char **data;
    size_t length;
} StringArray_t;


struct StringArray *StringArray_alloc();


void StringArray_dealloc(struct StringArray *arr);


size_t StringArray_add(struct StringArray *arr, char *str);


char *StringArray_get(struct StringArray *arr, size_t idx);


long NM_getFileSize(FILE *fp);

void NM_allocStringFromFile(char **s, FILE *fp);

struct StringArray *NM_readLines(char *string);

struct StringArray *NM_splitString(char *string, const char *delim);


#endif //PCH2CSD_UTILS_H
