int write_final_output(PasswordList* head, char* output_path){
    FILE *f = fopen(output_path, "w");
    if (f == NULL)
    {
        printf("Couldn't open file!\n");
        exit(1);
    }

    PasswordList *current = head;
    while (current != NULL){
        fprintf(f, "%s : %s\n", current->obj.username, current->obj.password);
        current = current->next;
    }

    fclose(f);

    return 0;
}
