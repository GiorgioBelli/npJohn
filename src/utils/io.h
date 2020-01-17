#define UTILS_IO_H 

#ifndef STDIO_H
    #define STDIO_H
    #include <stdio.h>
#endif

#ifndef MPI_H
    #define MPI_H
    #include <mpi.h>
#endif

#ifndef STDLIB_H
    #define STDLIB_H
    #include <stdlib.h>
#endif

#ifndef STRING_H
    #define STRING_H
    #include <string.h>
#endif

#ifndef HANDLER_H
    #include "../input-handler/input_handler.h"
#endif

/**
 * Writes the final output in a file on a given path.
 */
int write_final_output(PasswordList* headGuessed, PasswordList* headList, char* output_path, int worldRank, int worldSize);

#include "io.c"