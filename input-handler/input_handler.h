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
#define MAXCHAR 1000

typedef struct passwordList{  
    Password  obj; 
    struct passwordList *next; 
}PasswordList;

void addElement(PasswordList** head, Password* obj);
PasswordList* createStruct(char* fileName);

#include "input_handler.c"