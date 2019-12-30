#include "main.h"

// HINTS for compiling (with required links to static libraries):
// mpicc -o main.out main.c -pthread -lcrypt -lcrypto

bool incremental_flag = false;
int incremental_block_size = 100;
bool rule_flag = false;
int add_n = 0;
bool out_file_flag = false;
char* output_file_path;
CrackingStatus crackingStatus = {0,0,0};


int main(int argc, char const *argv[]) {

    handleUserOptions(argc, argv);


    ThreadData *data = initData();
    
    // Range ranges[] = {{48,57},{65,90},{97,122}};
    // char* res = NULL;
    // int len;
    // res = mapRangeIntoArray(&ranges,3,&len);

    // if(data->worldRank==0){
    //     for (int i = 0; i < len; i++)
    //     {
    //         printf("%d -> %c\n",i,res[i]);
    //     }
        
    // }
    

    // FILE* f0 = fopen("/home/giorgio/Desktop/f0","w+");
    // FILE* f1 = fopen("/home/giorgio/Desktop/f1","w+");

    // int word1[] = {61,61,60,0};
    // int word2[] = {61,61,60,1};
    // int * chk1;
    // int * chk2;
    // for (int i = 0; i < 122; i++)
    // {
    //     if(data->worldRank==0) {chk1 = parallel_incrementalNextWord(&word1,4,res,len,data->worldRank,data->worldSize);}
    //     if(data->worldRank==1) {chk2 = parallel_incrementalNextWord(&word2,4,res,len,data->worldRank,data->worldSize);}
        
    //     if(chk1 == NULL && data->worldRank==0){
    //         printf("%d -> parole finite",data->worldRank);fflush(stdout);
    //         return 0;
    //     }
    //     if(chk2 == NULL && data->worldRank==1){
    //         printf("%d -> parole finite",data->worldRank);fflush(stdout);
    //         return 0;
    //     }
    //     for (int j = 0; j < 4; j++)
    //     {
    //         if(data->worldRank==0){
    //             fprintf(f0,"%c",res[word1[j]]);

    //         }
    //         if(data->worldRank==1){
    //             fprintf(f1,"%c",res[word2[j]]);

    //         }

    //     }
    //     if(data->worldRank==0) fprintf(f0,"\n");
    //     if(data->worldRank==1) fprintf(f1,"\n");
    //     for (int j = 0; j < 4; j++)
    //     {
    //         if(data->worldRank==0){
    //             fprintf(f0,"-%d",word1[j]);

    //         }
    //         if(data->worldRank==1){
    //             fprintf(f1,"-%d",word2[j]);

    //         }

    //     }
    //     if(data->worldRank==0) fprintf(f0,"\n");
    //     if(data->worldRank==1) fprintf(f1,"\n");
    // }

    // if(data->worldRank==0) fclose(f0);
    // if(data->worldRank==1) fclose(f1);

    
    
    // MPI_Finalize();
    // free(res);
    // free(data);
    // return 0;

    crackingStatus.guess=data->worldRank;
    crackingStatus.try=data->worldRank;

    trace("Threads have been successfully started.\n", data->worldRank);

    // Run John the Ripper
    crackThemAll(data);
    
    // To prevent the listener thread to become a zombie thread, join it with its master thread
    pthread_join(data->threadId, NULL);

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
    int count = 10;

    trace("This simulation will last approximately 10 secs.", data->worldRank);
    trace("You can stop the program at any time by pressing 'q'.", data->worldRank);

    //TODO fix these variables, it's time to sleep for me... :)
    Range ranges[] = {{48,57},{65,90},{97,122}};
    char* res = NULL;
    int len; // is initialized by mapRangeIntoArray function
    res = mapRangeIntoArray(&ranges,3,&len); //maps ranges into one array
    int word[] = {39,45,37,51}; // this is the initial word
    int wordLen = sizeof(word)/sizeof(word[0]);

    int * chk;  // only used for check if incremental returns null, 
                // we shuold use word but the word's content will be lost

    word[3] += data->worldRank;

    while (data->shouldCrack) {

        if(incremental_flag){

            chk = parallel_incrementalNextWord(&word,wordLen,res,len,data->worldRank,data->worldSize);
            
            if(chk == NULL){
                printf("[%d] -> parole finite",data->worldRank);fflush(stdout);
                break;
            }

            /*
            
            TODO generate hash for word and check if it's equal to a password in list 
            
            */


        }
        // sleep(1);
    }

    if(data->worldRank==0) fclose(f0);
    if(data->worldRank==1) fclose(f1);
    free(res);

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
    int opt; 

    while((opt = getopt(argc, argv, ":o:r:i")) != -1)  
    {  
        switch(opt)  
        {  
            case 'o':  
                out_file_flag = true;
                output_file_path = calloc(sizeof(char),strlen(optarg)+1);
                strcpy(output_file_path,optarg);
                break;
            case 'r':
                rule_flag = true;
                // if optarg isn't a correct string atoi returns 0
                add_n = atoi(optarg); 
                break;  
            case 'i':
                incremental_flag = true;
                break; 
            case ':':  
                if (opt == 'o') {
                    printf("The option -o needs an output file as an argument.\n");  
                }
                if (opt == 'r') {
                    printf("The option -r needs an integer as an argument.\n");  
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
        int sendData[2] = {};
        getDataFromProcess(&sendData);

        // Main process gathers information processed by the others
        int *receiveBuffer = NULL;
        if (data->worldRank == ROOT) {
            receiveBuffer = malloc(sizeof(int)*data->worldSize*2);
        }
        MPI_Gather(&sendData, 2, MPI_INT, receiveBuffer, 2, MPI_INT, ROOT, MPI_COMM_WORLD); 
        if (data->worldRank == ROOT) {
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

