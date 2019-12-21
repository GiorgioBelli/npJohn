
#ifndef STDBOOL_H
    #include <stdbool.h>
#endif

#ifndef STDIO_H
    #include <stdio.h>
#endif


#ifndef STDLIB_H
    #include <stdlib.h>
#endif

#ifndef STRING_H
    #include <string.h>
#endif

#ifndef PASSWORD_H
    #include "../password/password.h"
#endif

#define MODES_H

typedef struct Range{
    char min;
    char max;
} Range;


int incremental(int maxWordLen,bool infinity, Range* ranges, int rangesLen);
char* incrementalNextWord(char* word, Range* ranges,int rangesLen);
bool dictWordCrack(Password* password, char* dictWord, HASH_TYPES hashType);
bool singleCrack(Password* password, HASH_TYPES hashType);


#include "modes.c"