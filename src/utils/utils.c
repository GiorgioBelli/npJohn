char** str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}


int* decodeRanges(char* string, int* lenArray) {
    char** tokens;
    //printf("string=[%s]\n\n", string);
    int* ranges = calloc(1, sizeof(int));
    int len = 0;
    tokens = str_split(string, ',');
    if (tokens)
    {
        int lastEnd = -1;
        int i;
        int numRanges = 0;
        for (i = 0; *(tokens + i); i++) {
            numRanges++;
        }
        for (i = 0; i < numRanges; i++)
        {
            char** ints  = str_split(tokens[i], '-');
            int start = atoi(ints[0]);
            free(ints[0]);
            //printf("%d\n", start);
            int end = atoi(ints[1]);
            free(ints[1]);
            //printf("%d\n", end);
            // if(start != lastEnd + 1) {
            //     // printf("Bad format\n");
            //     return NULL;
            // }
            // if (i == numRanges - 1 && end != 127) {
            // //    printf("Bad format\n");
            //     return NULL; 
            // }
            lastEnd = end;
            len += 2;
            ranges = realloc(ranges, sizeof(int)*len);
            ranges[len-2] = start;
            ranges[len-1] = end;
            free(ints);
            free(tokens[i]);
        }
        free(tokens);
    } else {
    // printf("Bad format\n");
    return NULL; 
    }


    *lenArray = len;

    return ranges;
}

