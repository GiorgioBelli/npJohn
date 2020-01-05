#ifndef STDIO_H
    #include <stdio.h>
#endif


#ifndef STDLIB_H
    #include <stdlib.h>
#endif

/**
 * Writes the final output in a file on a given path.
 */
int write_final_output(char* text, char* output_path);

#include "io.c"