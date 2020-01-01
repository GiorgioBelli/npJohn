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

#ifndef HANDLER_H
    #include "input-handler/input_handler.h"
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

#ifndef GETOPT_H
    #include <getopt.h>
#endif

#define ROOT 0
#define QUIT 'q'
#define STATUS '\n'

typedef struct threadData {
    int worldRank;
    int shouldCrack;
    int worldSize;
    pthread_t threadId;
    pthread_t firstThread;
    pthread_t thread2Id;
} ThreadData;

typedef struct crackingStatus{
    int guess;
    int try;
    time_t starting_time;
    char* currentWord;
} CrackingStatus;

void trace(char *msg, int rank);                                        // handy printf for MPI
void getDataFromProcess();                                              // data for checking the status of the program
int handleUserOptions(int argc, char const *argv[],ThreadData *data);   // handles program calls' options
int handleKeyPressed(char key, ThreadData *data);                       // what the name suggests
void *threadFun(void *vargp);                                           // called by each thread
void killThemAll();                                                     // cancel threads running in backgorund
void *crackThemAll(ThreadData *data);                                   // main logic of JTR
ThreadData *initData();                                                 // just for initialization purposes