
#ifndef STDIO_H
    #include <stdio.h>
#endif


#ifndef STDLIB_H
    #include <stdlib.h>
#endif

#ifndef STRING_H
    #include <string.h>
#endif


#ifndef ASSERT_H
    #include <assert.h>
#endif

char** str_split(char* a_str, const char a_delim);


/**
 * Decodes the ranges, given a well formatted string.
 * Returns array of integers, it has to be interpreted in pairs, the first pair is the first
 * range, the second pair the second range...
 * It puts the length of the array in the variable len.
 * Example of right format of accepted string ->   "0-3,4-8,9-38,39-127"
 * */
int* decodeRanges(char* string, int* lenArray);

#include "utils.c"
