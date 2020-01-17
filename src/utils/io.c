int write_final_output(PasswordList* headGuessed, PasswordList* headList, char* output_path, int worldRank, int worldSize){
    //FILE *f = fopen(output_path, "w");
    char tempName[15];
    sprintf(tempName, "temp_%d.txt", worldRank);

    FILE *f = fopen(tempName, "w");
    if (f == NULL){
        printf("Couldn't open file!\n");
        exit(1);
    }

    PasswordList *current = headGuessed;
    while (current != NULL){
        fprintf(f, "%s : %s\n", current->obj.username, current->obj.password);
        current = current->next;
    }
    fclose(f);

    MPI_Barrier(MPI_COMM_WORLD);

    if(worldRank == 0){
        FILE *ff = fopen(output_path, "w");
        current = headList;
        while (current != NULL){
            if( current->obj.hashType == NOPASSWORD_t ){ fprintf(ff, "%s : NOPASSWORD\n", current->obj.username); }
            current = current->next;
        }

        for (int i = 0; i <= worldSize ; i++){
            sprintf(tempName, "temp_%d.txt", worldRank);
            FILE *t = fopen(tempName, "w");

            char row[50];
            while (fgets(row, 50, t) != NULL){
                fprintf(ff, "%s", row);
            }
            fclose(t);
        }

        fclose(ff);
    }

    freePass(headList);
    freePass(headGuessed);
    
    return 0;
}
