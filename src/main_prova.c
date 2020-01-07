#include <mpi.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

const int PSW_FOUND = 0;
const int KEY_PRESSED = 1;

void *keyListener(void *vargp) {
    bool *shouldContinue = (bool *)vargp;
    while (1) {
        char c = fgetc(stdin);
        int worldSize;
        MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

        for (int i = 0; i < worldSize; i++) {
            MPI_Send(&c, 1, MPI_BYTE, i, KEY_PRESSED, MPI_COMM_WORLD);
        }
        if (c == 'q') {
            *shouldContinue = false;
            break;
        }
    }
    pthread_exit(NULL);
}

void *listener(void *vargp) {
    bool *shouldContinue = (bool *)vargp;
    int worldRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);
    while (1) {
        char c;
        MPI_Status status;
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        fflush(stdin);
        if (status.MPI_TAG == KEY_PRESSED) {
            MPI_Recv(&c, 1, MPI_BYTE, status.MPI_SOURCE, KEY_PRESSED,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (c == 'q') {
                *shouldContinue = false;
                break;
            } else if (c == '\n') {
                printf("Ciao io sono Gigi e sono il numero %d\n", worldRank);
                fflush(stdout);
            }
        } else if (status.MPI_TAG == PSW_FOUND) {
            int pswIndex;
            MPI_Recv(&pswIndex, 1, MPI_INT, status.MPI_SOURCE, PSW_FOUND,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("La password inviata è %d e io sono: %d\n", pswIndex,
                   worldRank);
            fflush(stdout);
            char buf[1024];
            snprintf(buf, sizeof(buf), "%d.txt", worldRank);
            FILE *fp = fopen(buf, "w+");
            fprintf(fp, "%d", pswIndex);
            fclose(fp);
        }
    }
    pthread_exit(NULL);
}

void notifyPasswordFound(int pswIndex) {
    int worldSize;
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
    for (int i = 0; i < worldSize; i++) {
        MPI_Send(&pswIndex, 1, MPI_INT, i, PSW_FOUND, MPI_COMM_WORLD);
    }
}

int main(int argc, char const *argv[]) {
    int worldRank;
    int worldSize;
    pthread_t threadId;
    pthread_t threadId2;

    bool shouldContinue = true;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
    // ---------------------------------------------

    pthread_create(&threadId, NULL, listener, &shouldContinue);
    if (worldRank == 0) {
        pthread_create(&threadId2, NULL, keyListener, &shouldContinue);
    }

    int count = 0;

    while (shouldContinue) {
        sleep(1);
        // printf("%d: Core num %d sta lavorando\n", shouldContinue, worldRank);
        // fflush(stdout);

        count++;
        if (count == 3) {
            int pswIndex = 42;
            if (worldRank == 0) {
                notifyPasswordFound(pswIndex);
            }
            // pthread_cancel(threadId);
            // if (worldRank == 0) {
            //    pthread_cancel(threadId2);
            //}
            break;
        }
    }

    // ---------------------------------------------
    pthread_join(threadId, NULL);
    if (worldRank == 0) {
        pthread_join(threadId2, NULL);
    }
    MPI_Finalize();
}