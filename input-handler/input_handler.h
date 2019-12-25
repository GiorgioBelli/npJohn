#ifndef STDIO_H
    #include <stdio.h>
#endif

#ifndef STDLIB_H
    #include <stdlib.h>
#endif

#ifndef STRING_H
    #include <string.h>
#endif

#define HANDLE_H
#define fileName "input-handler/passwd.txt"
#define MAXCHAR 1000

typedef struct passwordList{  
    Password  obj; 
    struct passwordList *next; 
}passwordList;

void addElement(passwordList** head, Password* obj);
passwordList* createStruct();

#include "input_handler.c"