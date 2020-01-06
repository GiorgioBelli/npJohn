
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

#ifndef DIGEST_H
    #include "../hash/digest.h"
#endif

#define MODES_H

typedef struct Range{
    char min;
    char max;
} Range;

typedef enum RULES{
    CONCATENATE_TWO,
    ADD_N_NUMBERS,
    NO_RULE,
} RULES;

#define TOO_LONG_WORD "TLW"
#define CRYPT_t_MAXSIZE 8


int incremental(int maxWordLen,bool infinity, Range* ranges, int rangesLen);
char* incrementalNextWord(char* word, Range* ranges,int rangesLen);
bool dictWordCrack(Password* password, char* dictWord, HASH_TYPES hashType,RULES ruleType, Range ranges[],int rangesLen,int incrementalLen,CrackingStatus* crackingStatus);
bool singleCrack(Password* password, HASH_TYPES hashType,CrackingStatus* crackingStatus);
char* mapRangeIntoArray(Range* ranges,int rangesLen,int* resLen);



#include "modes.c"