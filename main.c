#include <mpi.h>
#include "hash/digest.h"
#include "modes/modes.h"

int main(int argc, char *argv[]){

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
        // free(digestFactory("ciao","ab",CRYPT_t));
        // printf("digest sul primo come calcolato\n");
        
    // test incremental
        // Range ranges[] = {{48,57},{65,90},{97,122}};
        // if(sizeof(ranges)==0) return 0;
        // printf("%ld\n",sizeof(ranges)/sizeof(ranges[0]));
        // incremental(1,0,ranges,sizeof(ranges)/sizeof(ranges[0]));
        // printf("incremental sul code 2 calcolato\n");

    // test incremental
        Range ranges[] = {{48,57},{65,90},{97,122}};
        char* word = calloc(sizeof(char),2);
        if(sizeof(ranges)==0) return 0;

        strcpy(word,"x");
    
        while(word!=NULL){
            word = incrementalNextWord(word,ranges,sizeof(ranges)/sizeof(ranges[0]));
            if(word==NULL) return 0;
            printf("%s\n",word);
        }

    // }


    

    // Termina MPI
    // MPI_Finalize();

    return 0;
}