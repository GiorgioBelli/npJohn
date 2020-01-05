#include "main.h"

// Please keep these as global variables, we need them as variables in order to send them!
const int PSW_FOUND = 0;
const int KEY_PRESSED = 1;


// HINTS for compiling (with required links to static libraries):
// mpicc -o main.out main.c -pthread -lcrypt -lcrypto
bool incremental_flag = false;
int incremental_min_len = -1;
int incremental_max_len = -1;
bool rule_flag = false;
int add_n = -1;
bool out_file_flag = false;
char* dict;
char* output_file_path;
char* input_file_path;
CrackingStatus crackingStatus = {0,0,0,NULL};


int main(int argc, char const *argv[]) {

    ThreadData *data = initData();

    if(handleUserOptions(argc, argv, data)){
        MPI_Finalize();
        free(data);
        if(out_file_flag){
            free(output_file_path);
        }
        return 0;
    };

    crackingStatus.guess=data->worldRank;
    crackingStatus.try=data->worldRank;

    trace("Threads have been successfully started.\n", data->worldRank);

    // Run John the Ripper
    crackThemAll(data);
    
    // To prevent others threads to become zombie threads, join them with their master thread.
    pthread_join(data->threadId, NULL);
    if (data->worldRank==ROOT) {
        pthread_join(data->thread2Id, NULL);
    }

    // TODO: Before printing the msg below check if there is data to be saved into a file.
    trace("\nYour results were stored in the file ... \n", data->worldRank);

    // Terminate MPI and hence the program
    MPI_Finalize();
    free(data);
    if(out_file_flag){
        free(output_file_path);
    }

    return 0;
}

void *crackThemAll(ThreadData *data) {

    trace("Running JTR ... \n", data->worldRank);

    // printf("Cracking passwords in range [x,y] ...\n"); 
    
    // This variable can be removed, is here for simulating a job that finishes in 'count' seconds

    trace("You can stop the program at any time by pressing 'q'.", data->worldRank);

    PasswordList* passwordList = createStruct(input_file_path);
    Range ranges[] = {{48,57},{65,90},{97,122}};
    int rangesLen = sizeof(ranges)/sizeof(ranges[0]);

    //incremental mode 
    if(incremental_flag){

        char* res = NULL;
        int len; // is initialized by mapRangeIntoArray function
        res = mapRangeIntoArray(&ranges,rangesLen,&len); //maps ranges into one array
        int word[] = {39,45,37,51}; // this is the initial word
        int wordLen = sizeof(word)/sizeof(word[0]);

        int * chk;  // only used for check if incremental returns null, 
                    // we should use word but the word's content will be lost

        word[wordLen-1] += data->worldRank;

        while (data->shouldCrack) {

            chk = parallel_incrementalNextWord(&word,wordLen,res,len,data->worldRank,data->worldSize);
            
            if(chk == NULL){
                // printf("[%d] -> parole finite",data->worldRank);fflush(stdout);
                break;
            }


            char* digest = NULL;
            HASH_TYPES hashType = NONETYPE_t;
            while(passwordList != NULL){
                //hashType = getTypeHash(&(passwordList->obj));
                digest = realloc(digest,(sizeof(char)*2*getDigestLen(passwordList->obj.hashType)+1));
                digestFactory(word,passwordList->obj.salt, passwordList->obj.hashType, digest);

                printf("hash -> %s\n",passwordList->obj.hash);
                // if(strcmp(digest,passwordList->obj.hash)==0) {
                if(strcmp(digest,"password hash")==0) {
                    /*passwordFound(&(passwordList->obj),word,data)*/
                }
                passwordList = passwordList->next;
            }
        }
        // sleep(1);
        free(res);
    }
    else if(rule_flag){ //dictionary mode (eventually) with rules
        DictList* dictList = NULL; //TODO modify dictList type with DictList*

        while(dictList != NULL){
            char* digest = NULL;
            HASH_TYPES hashType = NONETYPE_t;

            while(passwordList != NULL){
                //hashType = getTypeHash(&(passwordList->obj));
                RULES rule = NO_RULE;
                if(add_n!=-1) rule = ADD_N_NUMBERS;
                
                //the block below needs the dict list implemented

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
    }
    else{ //fake execution

        int count = 10;
        trace("This simulation will last approximately 10 secs.", data->worldRank);

        while (data->shouldCrack && count>0) {
            
            count--;
            sleep(1);

        }
    }

    // Once here, the work is ended and the other threads are no longer necessary.
    killThemAll(data);
    return NULL;
}

void killThemAll(ThreadData *data) {
    pthread_cancel(data->threadId);
    if (data->thread2Id != NULL) {
        pthread_cancel(data->thread2Id);
    }
}

void passwordFound(Password* password,char* word,ThreadData* data){
    password->password = calloc(sizeof(char),strlen(word)+1);
    strcpy(password->password,word);
    notifyPasswordFound(data, password->password);  // notify other cores
    printMatch(password);
}

// This is a very inefficient way to bcast to all other nodes the found psw but I had no clue how.
// to manage to send mpi messages using different threads. This uses a miniprotocol for communicating:
// first the type of the msg is sent as well as the data that can be then safely parsed.
void notifyPasswordFound(ThreadData *data, char *clear_psw) {
    for (int i=0; i<data->worldSize; i++) {
        if (i!=data->worldRank) {
            MPI_Send(&PSW_FOUND, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&clear_psw, strlen(clear_psw), MPI_BYTE, i, 0, MPI_COMM_WORLD);
        }
    }
}

void printMatch(Password* password){
    /*
        TODO: implement password natch 
    */
}

// This function is run by each thread which listens for user input
void *threadFun(void *vargp) {
    ThreadData *data = (ThreadData *)vargp;
    while (1) {
        //sleep(0.2);   This is probably no more needed, each call inside if/else is a blocking call.
        char c;
        int msgType;
        if (data->worldRank == ROOT && pthread_self() == data->firstThread) {
            // The first thread (2) of core n.0 must listen for key pressed
            c = fgetc(stdin);
            for (int i=0; i<data->worldSize; i++) {
                MPI_Send(&KEY_PRESSED, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            }
            msgType = KEY_PRESSED;  // only this thread already knows the real value of msgType
        } else {
            // All the other threads must listen for an incoming msg
            MPI_Recv(&msgType, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        if (msgType == KEY_PRESSED) {
            if (handleKeyPressed(c, data)) {
                trace("\nQuitting the program ...", data->worldRank);
                // The user has pressed letter 'q' to exit the program
                return NULL;
            }
        } else if (msgType == PSW_FOUND) {
            MPI_Status status;
            int password_len;
            MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);    // probe the msg before collecting it
            MPI_Get_count(&status, MPI_INT, &password_len);     // get the msg size
            char *password = malloc(sizeof(char)*(password_len+1));
            MPI_Recv(&msgType, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            password[password_len] = '\0';
            markAsFound(password);
            free(password);
        }
    }   
}

void markAsFound(char *password) {
    // Mark the current given password as found
}


int handleUserOptions(int argc, char const *argv[],ThreadData *data) {
    int opt; 

    while(true)
    // while((opt = getopt(argc, argv, ":o:r:i:")) != -1)  
    {  

        int this_option_optind = optind ? optind : 1;
        int option_index = 0;
        static struct option long_options[] = {
            {"min-len", required_argument, 0,  0 },
            {"max-len", required_argument, 0,  0 },
            {"add-n"  , required_argument, 0,  0 },
        };

        opt = getopt_long(argc, argv, ":o:ri",long_options, &option_index);
        
        if (opt == -1) break;

                    
        switch(opt)  
        {  
            case 0:
                if(strcmp(long_options[option_index].name,"max-len")==0){
                    if(atoi(optarg) < 0){
                        trace("--max-len must be a positive integer\n",data->worldRank);
                        return 1;
                    }
                    incremental_max_len = atoi(optarg);
                }
                else if(strcmp(long_options[option_index].name,"min-len")==0){
                    if(atoi(optarg) < 0){
                        trace("--min-len must be a positive integer\n",data->worldRank);
                        return 1;
                    }
                    incremental_min_len = atoi(optarg);
                }
                else if(strcmp(long_options[option_index].name,"add-n")==0){
                    if(atoi(optarg) < 0){
                        trace("--add-n must be a positive integer\n",data->worldRank);
                        return 1;
                    }
                    add_n = atoi(optarg);
                }
                break;
            case 'o':  
                out_file_flag = true;
                output_file_path = calloc(sizeof(char),strlen(optarg)+1);
                strcpy(output_file_path,optarg);
                break;
            case 'r':
                rule_flag = true;
                // if optarg isn't a correct string atoi returns 0
                // add_n = atoi(optarg); 
                break;  
            case 'i':
                incremental_flag = true;
                break; 
            case ':':  
                if (opt == 'o') {
                    trace("Usage: The option -o needs an output file as an argument.\n",data->worldRank);  
                }
                if (opt == 'r') {
                    trace("Usage: The option -r needs an integer as an argument.\n",data->worldRank);  
                }
                return 1;  
            case '?':  
                printf("Usage: Unknown option: %c\n", optopt); 
                return 1;
        }  
        
    }

    if (optind == argc){
        printf("Usage: Missing parameter for input_file\n"); 
        return 1;
    }
    
    for(; optind < argc; optind++){      
        input_file_path = (char *)calloc(sizeof(char), strlen(argv[optind])+1);
        strncpy(input_file_path, argv[optind], strlen(argv[optind]));
        input_file_path[strlen(argv[optind])+1] = "\0";
    }  

    if(incremental_flag && rule_flag){
        trace("Usage: -i and -r cannot be used together.\n",data->worldRank);
        return 1;
    }

    if(!incremental_flag && (incremental_min_len!=-1 || incremental_max_len!=-1)){
        trace("Usage: --max-len and --min-len can only be used with -i option.\n",data->worldRank);
        return 1;
    }

    if((!rule_flag && add_n!=-1)){
        trace("Usage: --add-n can only be used with -r option.\n",data->worldRank);
        return 1;
    }

    if(rule_flag && (add_n==-1 /* || other rules*/) ){
        trace("Usage: -r needs a rule type parameter.\n    try -r -add-n=5.\n",data->worldRank);
        return 1;
    }

    return 0;
}

// Disclaimer: this function assumes WORLD_SIZE > 1
int handleKeyPressed(char key, ThreadData *data) {
    // Broadcast the character read ONCE to each MPI process
    if (pthread_self() == data->firstThread) {
        MPI_Bcast(&key, 1, MPI_BYTE, ROOT, MPI_COMM_WORLD);
    }

    if (key == QUIT) {
        data->shouldCrack = 0;
        return 1;

    } else if (key == STATUS) {
        // The core responsible for holding data is not the ROOT since it is running 3 threads in total
        // and could remain stucked when calling MPI_Gather (because would call it twice).
        const int CHOSEN_CORE = ROOT+1;    

        // Here we have the data to be sent from each process
        int sendData[2] = {};
        getDataFromProcess(&sendData);

        // Main process gathers information processed by the others
        int *receiveBuffer = NULL;
        if (data->worldRank == CHOSEN_CORE) {
            if ((receiveBuffer = malloc(sizeof(int)*data->worldSize*2)) == NULL) {
                printf("Some error occourred when allocating memory ...\n");
                MPI_Abort(MPI_COMM_WORLD, 1);
            }
        }
        MPI_Gather(&sendData, 2, MPI_INT, receiveBuffer, 2, MPI_INT, CHOSEN_CORE, MPI_COMM_WORLD); 
        if (data->worldRank == CHOSEN_CORE) {
            int guess = 0;
            int try = 0;
            for (int i=0; i<data->worldSize; i+=2) {
                    guess+=receiveBuffer[i];
                    try+=receiveBuffer[i+1];
            }
            printStatus(crackingStatus.starting_time,guess,try);
            free(receiveBuffer);
        }
    }
    return 0;
}

void printStatus(starting_time,guess,try){
    int elapsed_sec = time(0) - starting_time;

    int days = elapsed_sec/86400;
    int hours = (elapsed_sec-(days*86400))/3600;
    int minutes = (elapsed_sec - (hours*3600*86400)) / 60;
    int seconds = elapsed_sec % 60;

    printf("%dg %d:%d:%d:%d %dg/s\n",
        guess,
        days,
        hours,
        minutes,
        seconds,
        guess/(try+1)

    );

}

// This function is responsible for collecting the useful information to be printed for the status
void getDataFromProcess(int* sendData) {
    sendData[0] = crackingStatus.guess;
    sendData[1] = crackingStatus.try;

    return;
}


ThreadData *initData() {
    // This structure contains info about program status to be shared among threads
    ThreadData *data;
    if ((data = malloc(sizeof(ThreadData))) == NULL) {
        printf("Some error occourred when allocating memory ...\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    };
    
    // Default behaviour specifies the program to crack the passwords
    data->shouldCrack = 1;

    crackingStatus.starting_time = time(0);

    // Initialize MPI
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &(data->worldRank));

    MPI_Comm_size(MPI_COMM_WORLD, &(data->worldSize));

    if (pthread_create(&(data->threadId), NULL, threadFun, data)) {
        // Abort the execution if threads cannot be started
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    // If the thread has been correctly created, mark it as the first thread created
    // because we want to distinguish the 2 threads running threadFun() wihtin core num. 0
    data->firstThread = data->threadId;

    if (data->worldRank == ROOT && pthread_create(&(data->thread2Id), NULL, threadFun, data)) {
        // Same, but also for the second (3) thread of the main process.
        MPI_Abort(MPI_COMM_WORLD, 1);
    };
    return data;
}

// Useful function used to print just once messages onto screen
void trace(char *msg, int rank) {
    if (rank == ROOT) {
        puts(msg);
    }
}


// ------------------------------------ OLD CODE TO BE REMOVED HERE ------------------------------------

//int main(int argc, char const *argv[])
//{
//    setDebugPrints(1);
//
//    Password obj;
//    obj.username = "user";
//    obj.hash = "mmeiners:e7hvcqLV0YUmQ:53212:532:Cpr E 532 Student:/home/issl/532/mmeiners:/bin/tcsh";
//
//    free(digestFactory("ciao","ab", getTypeHash(obj)));
//
//}


//int main(int argc, char *argv[]){
//
    //char* output_file_path;
    //int opt; 

    //while((opt = getopt(argc, argv, ":o:")) != -1)  
    //{  
        //switch(opt)  
        //{  
            //case 'o':  
                //output_file_path = optarg; 
                //break;  
            //case ':':  
                //if (opt == 'o') {
                    //printf("The option -o needs an output file as an argument.\n");  
                //}
                //break;  
            //case '?':  
                //printf("Unknown option: %c\n", optopt); 
                //break;  
        //}  
    //}  

    // // Inizializza MPI
    // MPI_Init(&argc, &argv);

    // // Numero di processi disponibili
    // int world_size;
    // MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // // Rank del processo - Numero del core
    // int world_rank;
    // MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // // Nome del processo - Come un hostname
    // char processor_name[MPI_MAX_PROCESSOR_NAME];
    // int name_len;
    // MPI_Get_processor_name(processor_name, &name_len);

    // if(world_rank == 0){

    // test digestFactory
        // setDebugPrints(1);
        // free(digestFactory("lastclas","zY",MD5_t));
        
    // test incremental
        // Range ranges[] = {{48,57},{65,90},{97,122}};
        // if(sizeof(ranges)==0) return 0;
        // printf("%ld\n",sizeof(ranges)/sizeof(ranges[0]));
        // incremental(1,0,ranges,sizeof(ranges)/sizeof(ranges[0]));
        // printf("incremental sul code 2 calcolato\n");

    // test incremental
        // Range ranges[] = {{48,57},{65,90},{97,122}};
        // char* word = calloc(sizeof(char),2);
        // if(sizeof(ranges)==0) return 0;

        // strcpy(word,"x");
    
        // while(word!=NULL){
        //     word = incrementalNextWord(word,ranges,sizeof(ranges)/sizeof(ranges[0]));
        //     if(word==NULL) return 0;
        //     printf("%s\n",word);
        // }

    // test singleModeciao
        // Password pwd = {"ciao","hash","lastclas"};

    // }


    

    // Termina MPI
    // MPI_Finalize();
    //return 0;
//}

