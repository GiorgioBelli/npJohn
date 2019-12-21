int write_final_output(char* text, char* output_path)
{
    FILE *f = fopen(output_path, "w");
    if (f == NULL)
    {
        printf("Couldn't open file!\n");
        exit(1);
    }

    fprintf(f, "%s",text);
    fclose(f);

    return 0;
}

int add(int a, int b) {
    return a + b;
}