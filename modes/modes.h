#include <stdbool.h>

typedef struct Range{
    char min;
    char max;
} Range;


int incremental(int maxWordLen,bool infinity, Range* ranges, int rangesLen);

#include "modes.c"