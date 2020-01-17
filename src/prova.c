#ifndef UNISTD_H
    #include <unistd.h>
#endif

#ifndef PTRHEAD_H
    #include <pthread.h>
#endif

#ifndef TIME_H
    #include <time.h>
#endif

#ifndef GETOPT_H
    #include <getopt.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
typedef struct dictList{
    char* word;
    struct dictList* next;
}DictList;

int main(){
    char str[12];
    sprintf(str, "temp_%d.txt", 5);
    sprintf(str, "zamp_%d.txt", 5);
    printf("%s", str);

    return 0;
}