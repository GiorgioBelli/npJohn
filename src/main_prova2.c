#include "main.h"

// Please keep these as global variables, we need them as variables in order to
// send them!
const int PSW_FOUND = 0;
const int KEY_PRESSED = 1;

// HINTS for compiling (with required links to static libraries):
// mpicc -o main.out main.c -pthread -lcrypt -lcrypto

// HINTS for running on lan:
// mpirun -np N ./main.out input-handler/passwd.txt
// mpirun -np 12 -hosts <master_ip>,<slave_1_ip>,...,<slave_m_ip> ./main.out
// input-handler/passwd.txt

bool incremental_flag = false;
int incremental_min_len = -1;
int incremental_max_len = -1;
bool rule_flag = false;
int add_n = -1;
bool out_file_flag = false;
char *dict;
char *output_file_path;
char *input_file_path;
Range *ranges = NULL;
int rangesLen = 0;
CrackingStatus crackingStatus = {0, 0, 0, NULL};

int main(int argc, char const *argv[]) {
    ThreadData *data = initData();

    if (handleUserOptions(argc, argv, data)) {
        MPI_Finalize();
        free(data);
        if (out_file_flag) {
            free(output_file_path);
        }
        return 1;
    };

    crackingStatus.guess = data->worldRank;
    crackingStatus.attempt = data->worldRank;

    trace("Threads have been successfully started.\n", data->worldRank);

    // int count = 0;
    // while (data->shouldCrack && count++ < 6) {
    // sleep(1);
    // if (data->worldRank == ROOT) {
    // printf("Current sec is %d\n", count);
    // fflush(stdout);
    //      }
    //
    // if (count == 2 && data->worldRank == ROOT) {
    //  int msg = 42;
    //  printf("La password inviata è %d e io sono: %d\n", msg,
    //         data->worldRank);
    //  fflush(stdout);
    //   for (int i = 0; i < data->worldSize; i++) {
    //       if (i != data->worldRank) {
    //             MPI_Send(&msg, 1, MPI_INT, i, PSW_FOUND, MPI_COMM_WORLD);
    //        }
    //     }
    //  }
    //}

    // Run John the Ripper
    crackThemAll(data);

    // TODO: Before printing the msg below check if there is data to be saved
    // into a file.
    trace("\nYour results were stored in the file ... \n", data->worldRank);

    // Terminate MPI and hence the program
    MPI_Finalize();
    free(data);
    free(ranges);
    if (out_file_flag) {
        free(output_file_path);
    }
    return 0;
}

void *listener(void *vargp) {
    ThreadData *data = (ThreadData *)vargp;
    while (data->shouldCrack) {
        int msgType;
        MPI_Status status;
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        if (status.MPI_TAG == PSW_FOUND) {
            int pswIndex;
            MPI_Recv(&pswIndex, 1, MPI_INT, status.MPI_SOURCE, status.MPI_TAG,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            markAsFound(pswIndex, data);
        } else if (status.MPI_TAG == KEY_PRESSED) {
            char c;
            MPI_Recv(&c, 1, MPI_BYTE, status.MPI_SOURCE, status.MPI_TAG,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (c == QUIT) {
                data->shouldCrack = false;
                printf("Quitting..\n");
                fflush(stdout);
                pthread_exit(NULL);
            } else {
                displayInfo(data);
            }
        }
    }
    return NULL;
}

void *keyListener(void *vargp) {
    ThreadData *data = (ThreadData *)vargp;
    while (data->shouldCrack) {
        char c = getchar();
        for (int i = 0; i < data->worldSize; i++) {
            MPI_Send(&c, 1, MPI_BYTE, i, KEY_PRESSED, MPI_COMM_WORLD);
        }
        if (c == 'q') {
            while ((c = getchar()) != '\n' && c != EOF)
                ;  // clear stdin buffer
            data->shouldCrack = false;
            printf("Quitting..\n");
            fflush(stdout);
            pthread_exit(NULL);
        } else {
            // MPI_Send(&c, 1, MPI_BYTE, i, KEY_PRESSED, MPI_COMM_WORLD); //
            // todo
        }
    }
    return NULL;
}

void displayInfo(ThreadData *data) {
    // Here we have the data to be sent from each process
    int sendData[2] = {};
    getDataFromProcess(&sendData);

    // Main process gathers information processed by the others
    int *receiveBuffer = NULL;

    if (data->worldRank == ROOT &&
        (receiveBuffer = malloc(sizeof(int) * data->worldSize * 2)) == NULL) {
        printf("Some error occourred when allocating memory ...\n");
        fflush(stdout);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    MPI_Gather(&sendData, 2, MPI_INT, receiveBuffer, 2, MPI_INT, ROOT,
               MPI_COMM_WORLD);
    if (data->worldRank == ROOT) {
        int guess = 0;
        int attempt = 0;

        for (int i = 0; i < data->worldSize; i += 2) {
            guess += receiveBuffer[i];
            attempt += receiveBuffer[i + 1];
        }
        printStatus(crackingStatus.starting_time, guess, attempt);
        free(receiveBuffer);
    }
}

void printStatus(starting_time, guess, attempt) {
    int elapsed_sec = time(0) - starting_time;

    int days = elapsed_sec / 86400;
    int hours = (elapsed_sec - (days * 86400)) / 3600;
    int minutes = (elapsed_sec - (hours * 3600 * 86400)) / 60;
    int seconds = elapsed_sec % 60;

    printf("%dg %d:%d:%d:%d %dg/s %ldt/s %s\n", guess, days, hours, minutes,
           seconds, guess / (attempt + 1), (long)attempt / elapsed_sec,
           crackingStatus.currentWord);
    fflush(stdout);
}

// This function is responsible for collecting the useful information to be
// printed for the status
void getDataFromProcess(int *sendData) {
    sendData[0] = crackingStatus.guess;
    sendData[1] = crackingStatus.attempt;

    return;
}

ThreadData *initData() {
    // This structure contains info about program status to be shared among
    // threads
    ThreadData *data;
    if ((data = malloc(sizeof(ThreadData))) == NULL) {
        printf("Some error occourred when allocating memory ...\n");
        fflush(stdout);
        MPI_Abort(MPI_COMM_WORLD, 1);
    };
    // Default behaviour specifies the program to crack the passwords
    data->shouldCrack = 1;

    crackingStatus.starting_time = time(0);

    // Initialize MPI
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &(data->worldRank));
    MPI_Comm_size(MPI_COMM_WORLD, &(data->worldSize));

    if (pthread_create(&(data->threadId), NULL, listener, data)) {
        // Abort the execution if threads cannot be started
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    data->firstThread = data->threadId;

    if (data->worldRank == ROOT &&
        pthread_create(&(data->thread2Id), NULL, keyListener, data)) {
        // Same, but also for the second (3) thread of the main process.
        MPI_Abort(MPI_COMM_WORLD, 1);
    };

    return data;
}

void killThemAll(ThreadData *data) {
    pthread_cancel(data->threadId);
    if (data->worldRank == ROOT) {
        pthread_cancel(data->thread2Id);
    }

    pthread_join(data->threadId, NULL);
    if (data->worldRank == ROOT) {
        pthread_join(data->thread2Id, NULL);
    }
}

void *crackThemAll(ThreadData *data) {
    trace(
        "Running JTR ... \n\nYou can stop the program at any time by pressing "
        "'q'.",
        data->worldRank);

    // printf("Cracking passwords in range [x,y] ...\n");

    PasswordList *passwordList = createStruct(input_file_path);
    // Range ranges[] = {{48,57},{65,90},{97,122}};
    // Range ranges[] = {{97,122}};
    // int rangesLen = sizeof(ranges)/sizeof(ranges[0]);

    if (incremental_flag) {
        char *res = NULL;
        int resLen;  // is initialized by mapRangeIntoArray function
        res = mapRangeIntoArray(ranges, rangesLen,
                                &resLen);  // maps ranges into a linear array
        if (incremental_min_len == -1) incremental_min_len = 1;
        if (incremental_max_len == -1)
            incremental_min_len = 8;  // if not provided = 8

        char *alphaWord = NULL;

        for (int l = incremental_min_len; l <= incremental_max_len; l++) {
            int *word =
                calloc(sizeof(int), l);  // this is the initial word of charset,
                                         // initialized at 0 with calloc

            alphaWord = wordFromRange(word, alphaWord, res, l);

            int *chk;  // only used for check if incremental returns null,
                       // we should use word but the word's content will be lost

            word[l - 1] += data->worldRank;

            int counter = 0;

            char *digest = NULL;
            HASH_TYPES hashType = NONETYPE_t;

            PasswordList *passwordListPointer = passwordList;
            while (passwordListPointer != NULL) {
                counter++;
                hashType = getTypeHash(&(passwordListPointer->obj));
                digest = realloc(
                    digest,
                    (sizeof(char) * 2 *
                         getDigestLen(passwordListPointer->obj.hashType) +
                     1));
                digestFactory(alphaWord, passwordListPointer->obj.salt,
                              passwordListPointer->obj.hashType, digest);

                if (strcmp(digest, passwordListPointer->obj.hash) == 0) {
                    passwordFound(&(passwordListPointer->obj), counter,
                                  alphaWord, data);
                }
                passwordListPointer = passwordListPointer->next;
            }

            while (data->shouldCrack) {
                chk = parallel_incrementalNextWord(
                    word, l, res, resLen, data->worldRank, data->worldSize);

                alphaWord = wordFromRange(word, alphaWord, res, l);
                crackingStatus.attempt++;
                crackingStatus.currentWord = alphaWord;

                if (chk == NULL) {
                    // printf("[%d] -> parole
                    // finite",data->worldRank);fflush(stdout);
                    break;
                }

                counter = 0;

                passwordListPointer = passwordList;
                while (passwordListPointer != NULL) {
                    counter++;
                    hashType = getTypeHash(&(passwordListPointer->obj));
                    digest = realloc(
                        digest,
                        (sizeof(char) * 2 *
                             getDigestLen(passwordListPointer->obj.hashType) +
                         1));
                    digestFactory(alphaWord, passwordListPointer->obj.salt,
                                  passwordListPointer->obj.hashType, digest);

                    if (strcmp(digest, passwordListPointer->obj.hash) == 0) {
                        printf("found password with index: %d\n", counter);
                        fflush(stdout);
                        passwordFound(&(passwordListPointer->obj), counter,
                                      alphaWord, data);
                    }
                    passwordListPointer = passwordListPointer->next;
                }
                // if(data->worldRank==0) printf("[%d]
                // %s\n",data->worldRank,alphaWord);fflush(stdout);
            }
            free(word);
            free(digest);
        }
        // sleep(1);
        free(alphaWord);
        free(res);

    } else if (rule_flag) {         // dictionary mode (eventually) with rules
        DictList *dictList = NULL;  // TODO modify dictList type with DictList*

        while (dictList != NULL) {
            char *digest = NULL;
            HASH_TYPES hashType = NONETYPE_t;

            while (passwordList != NULL) {
                // hashType = getTypeHash(&(passwordList->obj));
                RULES rule = NO_RULE;
                if (add_n != -1) rule = ADD_N_NUMBERS;

                // the block below needs the dict list implemented

                // if(dictWordCrack(
                //     passwordList->obj,
                //     dictList->obj,
                //     passwordList->obj.hashType,
                //     rule,
                //     ranges,
                //     rangesLen,
                //     add_n,
                //     &crackingStatus
                // )){
                //     /* passwordFound(...) */
                // }

                passwordList = passwordList->next;
            }
        }
    } else {  // fake execution

        int count = 10;
        trace("This simulation will last approximately 10 secs.",
              data->worldRank);

        while (data->shouldCrack && count > 0) {
            count--;
            sleep(1);
        }
    }

    // Once here, end of work and other threads are no longer necessary.
    killThemAll(data);

    return NULL;
}

// reverse the mapping on the ranges into a string
char *wordFromRange(int word[], char *resultString, char map[], int wordlen) {
    resultString = (char *)realloc(resultString, sizeof(char) * (wordlen + 1));
    for (int i = 0; i < wordlen; i++) {
        resultString[i] = map[word[i]];
    }
    resultString[wordlen] = '\0';

    return resultString;
}

void passwordFound(Password *password, int index, char *word,
                   ThreadData *data) {
    password->password = calloc(sizeof(char), strlen(word) + 1);
    strcpy(password->password, word);
    crackingStatus.guess++;
    markAsFound(index, data);
    notifyPasswordFound(data, index);  // notify other cores
    printMatch(password);
}

// This is a very inefficient way to bcast to all other nodes the found psw but
// I had no clue how. to manage to send mpi messages using different threads.
// This uses a miniprotocol for communicating: first the type of the msg is sent
// as well as the data that can be then safely parsed.
void notifyPasswordFound(ThreadData *data, int passwordIndex) {
    for (int i = 0; i < data->worldSize; i++) {
        if (i != data->worldRank) {
            MPI_Send(&passwordIndex, 1, MPI_INT, i, PSW_FOUND, MPI_COMM_WORLD);
        }
    }
}

void printMatch(Password *password) {
    printf("%s: %s\n", password->username, password->password);
    fflush(stdout);
}

void markAsFound(int passwordIndex, ThreadData *data) {
    // TODO this function should move the password found from passwordList to
    // foundPasswordList
    printf("[%d] marked the password with index %d as FOUND.\n",
           data->worldRank, passwordIndex);
    fflush(stdout);
}

int handleUserOptions(int argc, char const *argv[], ThreadData *data) {
    int opt;

    while (true)
    // while((opt = getopt(argc, argv, ":o:r:i:")) != -1)
    {
        int this_option_optind = optind ? optind : 1;
        int option_index = 0;
        static struct option long_options[] = {
            {"min-len", required_argument, 0, 0},
            {"max-len", required_argument, 0, 0},
            {"add-n", required_argument, 0, 0},
            {"charset", required_argument, 0, 0},
        };

        opt = getopt_long(argc, argv, ":o:ri", long_options, &option_index);

        if (opt == -1) break;

        switch (opt) {
            case 0:
                if (strcmp(long_options[option_index].name, "max-len") == 0) {
                    if (atoi(optarg) < 0) {
                        trace("--max-len must be a positive integer\n",
                              data->worldRank);
                        return 1;
                    }
                    incremental_max_len = atoi(optarg);
                } else if (strcmp(long_options[option_index].name, "min-len") ==
                           0) {
                    if (atoi(optarg) < 0) {
                        trace("--min-len must be a positive integer\n",
                              data->worldRank);
                        return 1;
                    }
                    incremental_min_len = atoi(optarg);
                } else if (strcmp(long_options[option_index].name, "add-n") ==
                           0) {
                    if (atoi(optarg) < 0) {
                        trace("--add-n must be a positive integer\n",
                              data->worldRank);
                        return 1;
                    }
                    add_n = atoi(optarg);
                } else if (strcmp(long_options[option_index].name, "charset") ==
                           0) {
                    int rlen;
                    int *r = decodeRanges(optarg, &rlen);
                    rangesLen = (int)rlen / 2;  // number of couples
                    if (r == NULL) {
                        trace(
                            "Usage: --charset bad format:\n    try with: "
                            "--charset=48-57,65-90,97-122.\n",
                            data->worldRank);
                        return 1;
                    }
                    ranges = calloc(sizeof(Range), rangesLen);
                    for (int i = 0; i < rlen; i += 2) {
                        ranges[i / 2].min = r[i];
                        ranges[i / 2].max = r[i + 1];
                    }

                    free(r);
                } else {
                    trace("Usage: Unknow option\n", data->worldRank);
                    return 1;
                }
                break;
            case 'o':
                out_file_flag = true;
                output_file_path = calloc(sizeof(char), strlen(optarg) + 1);
                strcpy(output_file_path, optarg);
                break;
            case 'w':
                rule_flag = true;
                // if optarg isn't a correct string atoi returns 0
                // add_n = atoi(optarg);
                break;
            case 'i':
                incremental_flag = true;
                break;
            case ':':
                if (opt == 'o') {
                    trace(
                        "Usage: The option -o needs an output file as an "
                        "argument.\n",
                        data->worldRank);
                    return 1;
                }
                if (opt == 'r') {
                    trace(
                        "Usage: The option -r needs an integer as an "
                        "argument.\n",
                        data->worldRank);
                    return 1;
                }
                return 1;
            case '?':
                printf("Usage: Unknown option: %c\n", optopt);
                fflush(stdout);
                return 1;
        }
    }

    if (optind == argc) {
        trace("Usage: Missing parameter for input_file\n", data->worldRank);
        return 1;
    }

    for (; optind < argc; optind++) {
        input_file_path =
            (char *)calloc(sizeof(char), strlen(argv[optind]) + 1);
        strncpy(input_file_path, argv[optind], strlen(argv[optind]));
        input_file_path[strlen(argv[optind]) + 1] = "\0";
    }

    if (incremental_flag && rule_flag) {
        trace("Usage: -i and -r cannot be used together.\n", data->worldRank);
        return 1;
    }

    if (!incremental_flag &&
        (incremental_min_len != -1 || incremental_max_len != -1)) {
        trace(
            "Usage: --max-len and --min-len can only be used with -i option.\n",
            data->worldRank);
        return 1;
    }

    if ((!rule_flag && add_n != -1)) {
        trace("Usage: --add-n can only be used with -r option.\n",
              data->worldRank);
        return 1;
    }

    if (rule_flag && (add_n == -1 /* || other rules*/)) {
        trace("Usage: -r needs a rule type parameter.\n    try -r -add-n=5.\n",
              data->worldRank);
        return 1;
    }

    return 0;
}

// Useful function used to print just once messages onto screen
void trace(char *msg, int rank) {
    if (rank == ROOT) {
        puts(msg);
        fflush(stdout);
    }
}