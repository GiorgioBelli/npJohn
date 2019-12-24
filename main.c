#include "main.h"

// HINTS for compiling (with required links to static libraries):
// mpicc -o main.out main.c -pthread -lcrypt -lcrypto

int main(int argc, char const *argv[]) {

<<<<<<< HEAD
    free(digestFactory("ciao","ab", getHashType(obj)));
=======
    handleUserOptions(argc, argv);
>>>>>>> 9a0f9cf9f2465020456af1221e78bfa5a7bcef6d

    ThreadData *data = initData();

    trace("Threads have been successfully started.\n", data->worldRank);

    // Run John the Ripper
    crackThemAll(data);
    
    // To prevent the listener thread to become a zombie thread, join it with its master thread
    pthread_join(data->threadId, NULL);

    trace("\nYour results were stored in the file ... \n", data->worldRank);

    // Terminate MPI and hence the program
    MPI_Finalize();
    free(data);
    return 0;
}

void *crackThemAll(ThreadData *data) {

    trace("Running JTR ... \n", data->worldRank);

    // printf("Cracking passwords in range [x,y] ...\n"); 
    
    // This variable can be removed, is here for simulating a job that finishes in 'count' seconds
    int count = 10;

    trace("This simulation will last approximately 10 secs.", data->worldRank);
    trace("You can stop the program at any time by pressing 'q'.", data->worldRank);

    while (count-- && data->shouldCrack) {

        // Heavy work here
        sleep(1);
    }

    // The main thread has finished the work therefore stop its listener thread
    pthread_cancel(data->threadId);
    return NULL;
}

// This function is run by each thread which listens for user input
void *threadFun(void *vargp) {
    ThreadData *data = (ThreadData *)vargp;
    while (1) {
        sleep(0.2);
        char c;
        if (data->worldRank == ROOT) {
            // Listen for key pressed
            c = fgetc(stdin);
        }
        if (handleKeyPressed(c, data)) {
            // The user has pressed letter 'q' to exit the program
            return NULL;
        };
    }
}

int handleUserOptions(int argc, char const *argv[]) {
    char* output_file_path;
    int opt; 

    while((opt = getopt(argc, argv, ":o:")) != -1)  
    {  
        switch(opt)  
        {  
            case 'o':  
                output_file_path = optarg; 
                break;  
            case ':':  
                if (opt == 'o') {
                    printf("The option -o needs an output file as an argument.\n");  
                }
                break;  
            case '?':  
                printf("Unknown option: %c\n", optopt); 
                break;  
        }  
    }  

    return 0;
}

int handleKeyPressed(char key, ThreadData *data) {
    // Broadcast the character read to any MPI process
    MPI_Bcast(&key, 1, MPI_BYTE, ROOT, MPI_COMM_WORLD);

    if (key == QUIT) {
        data->shouldCrack = 0;
        return 1;

    } else if (key == STATUS) {
        // Here we have the data to be sent from each process
        int sendData = getDataFromProcess();

        // Main process gathers information processed by the others
        int *receiveBuffer = NULL;
        if (data->worldRank == ROOT) {
            receiveBuffer = malloc(sizeof(int)*4);
        }
        MPI_Gather(&sendData, 1, MPI_INT, receiveBuffer, 1, MPI_INT, ROOT, MPI_COMM_WORLD); 
        if (data->worldRank == ROOT) {
            for (int i=0; i<4; i++) {
                printf("Received data at index %d is %d. \n", i, receiveBuffer[i]);
            }
            free(receiveBuffer);
        }
    }
    return 0;
}

// This function is responsible for collecting the useful information to be printed for the status
int getDataFromProcess() {
    // TODO: implement the logic for collecting data here
    return 42;
}

ThreadData *initData() {
    // This structure contains info about program status to be shared among threads
    ThreadData *data;
    if ((data = malloc(sizeof(ThreadData))) == NULL) {
        printf("Some error occourred when allocating memory ...\n");
    };
    
    // Default behaviour specifies the program to crack the passwords
    data->shouldCrack = 1;

    // Initialize MPI
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &(data->worldRank));

    if (pthread_create(&(data->threadId), NULL, threadFun, data)) {
        // Abort the execution if threads cannot be started
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

