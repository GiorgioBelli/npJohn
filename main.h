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

#ifndef TIME_H
    #include <time.h>
#endif

#define ROOT 0
#define QUIT 'q'
#define STATUS '\n'

typedef struct threadData {
    int worldRank;
    int shouldCrack;
    int worldSize;
    pthread_t threadId;
} ThreadData;

typedef struct crackingStatus{
    int guess;
    int try;
    time_t starting_time;
} CrackingStatus;

void trace(char *msg, int rank);                    // handy printf for MPI
void getDataFromProcess();                           // data for checking the status of the program
int handleUserOptions(int argc, char const *argv[]);// handles program calls' options
int handleKeyPressed(char key, ThreadData *data);   // what the name suggests
void *threadFun(void *vargp);                       // called by each thread
void *crackThemAll(ThreadData *data);               // main logic of JTR
ThreadData *initData();                             // just for initialization purposes