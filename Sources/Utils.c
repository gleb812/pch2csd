//
// Created by Ech on 4/14/17.
//

#include <stdlib.h>
#include "Utils.h"

struct StringArray *StringArray_alloc() {
    struct StringArray *arr = (struct StringArray *) malloc(sizeof(StringArray_t));
    arr->data = NULL;
    arr->length = 0;
    return arr;
}

void StringArray_dealloc(struct StringArray *arr) {
    size_t i;
    for (i = 0; i < arr->length; i++) {
        free(arr->data[i]);
    }
    free(arr);
}

unsigned long StringArray_add(struct StringArray *arr, char *str) {
    char **oldData = arr->data;
    char **newData = (char **) malloc(sizeof(char *) * (arr->length + 1));

    size_t i;
    for (i = 0; i < arr->length; i++) {
        newData[i] = oldData[i];
    }
    char *newStr = (char *) malloc(strlen(str) + 1);
    strcpy(newStr, str);
    newData[arr->length] = newStr;
    free(oldData);

    arr->data = newData;
    arr->length++;
    return arr->length;
}

char *StringArray_get(struct StringArray *arr, size_t idx) {
    if (idx >= arr->length) {
        return NULL;
    }
    return arr->data[idx];
}

long NM_getFileSize(FILE *fp) {
    long pos = ftell(fp);
    fseek(fp, 0L, SEEK_END);
    long size = ftell(fp);
    fseek(fp, pos, SEEK_SET);
    return size;
}

void NM_allocStringFromFile(char **s, FILE *fp) {
    fseek(fp, 0L, SEEK_SET);

    long len = NM_getFileSize(fp);
    *s = (char *) malloc(sizeof(char) * (len + 1));

    char c, i = 0;
    while ((c = (char) getc(fp)) != EOF) {
        if (c == '\r') {
            continue;
        }
        (*s)[i] = c;
        i++;
    }
    (*s)[i] = '\0';
}

struct StringArray *NM_readLines(char *string) {
    struct StringArray *arr = StringArray_alloc();

    char *ptr = string;
    size_t wbeg = 0, wend = 0;
    while (true) {
        if (*ptr == '\n' || *ptr == '\0') {
            size_t len = wend - wbeg;

            char *s = (char *) malloc(sizeof(char) * (len + 1));
            strncpy(s, ptr - len, len);
            strcat(s, "\0");
            StringArray_add(arr, s);
            free(s);

            wbeg = wend + 1;
            if (*ptr == '\0') {
                break;
            }
        }
        wend++;
        ptr++;
    }
    return arr;
}

struct StringArray *NM_splitString(char *string, const char *delim) {
    struct StringArray *arr = StringArray_alloc();
    char *tmpstr = (char *) malloc(sizeof(char) * (strlen(string) + 1));
    char *token;
    token = strtok(string, delim);
    while (token != NULL) {
        StringArray_add(arr, token);
        token = strtok(NULL, string);
    }
    free(tmpstr);
    return arr;
}

