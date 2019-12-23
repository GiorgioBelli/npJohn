#ifndef MPI_H
    #include <mpi.h>
#endif

#ifndef HASH_DIGEST_H
    #include "hash/digest.h"
#endif

#ifndef MODES_H
    #include "modes/modes.h"
#endif

#ifndef UTILS_H
    #include "utils/io.h"
#endif

#ifndef UNISTD_H
    #include <unistd.h>
#endif

#ifndef PTRHEAD_H
    #include <pthread.h>
#endif

#define ROOT 0
#define QUIT 'q'
#define STATUS '\n'

typedef struct threadData {
    int worldRank;
    int shouldCrack;
    pthread_t threadId;
} ThreadData;


void trace(char *msg, int rank);                    // handy printf for MPI
int getDataFromProcess();                           // data for checking the status of the program
int handleKeyPressed(char key, ThreadData *data);   // what the name suggests
void *threadFun(void *vargp);                       // called by each thread
void *crackThemAll(ThreadData *data);               // main logic of JTR
ThreadData *initData();                             // just for initialization purposes