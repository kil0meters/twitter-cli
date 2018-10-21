#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char *get_string_at_index(const char *src, int start, int len) {
    char *res = malloc(len + 1);
    memcpy(res, src + start, len);
    res[len + 1] = '\0';
    return res;
}

char *cat_strs(const char *src, const char *str) {
    int bufsize = strlen(src) + strlen(str) + 1;
    char *res = malloc(bufsize);
    snprintf(res, bufsize, "%s%s", src, str);
    return res;
}
