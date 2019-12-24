int incremental(int maxWordLen,bool infinity, Range* ranges,int rangesLen){
    // int maxWordLen = 2;
    // bool infinity = true;
    // Range ranges[] = {{48,57},{65,90},{97,122}};

    if(rangesLen==0) return 0; // non ci sono parole da generare

    // int rangesLen = sizeof(ranges)/sizeof(ranges[0]);
    
    char* word = NULL;
    char* lastword = NULL;
    int wordlen=0;


    for (int j = 0; j < maxWordLen; j++){
        wordlen++;
        word = (char*) realloc(word,sizeof(char)*wordlen+1);
        lastword = (char*) realloc(lastword,sizeof(char)*wordlen+1);


        for (int i = 0; i < wordlen; i++){
            word[i] = ranges[0].min;
            lastword[i] = ranges[rangesLen-1].max;
        
        }
        word[wordlen] = '\0';
        lastword[wordlen] = '\0';
        
        while(true){
            for (int k = 0; k < rangesLen; k++){
                int cont = 0;

                while( word[wordlen-1] != ranges[k].max ){
                    word[wordlen-1]=ranges[k].min+cont;

                    /* incremental word */
                    printf("%s\n",word);

                    cont++;
                }
            }
            
            if (strcmp(word,lastword)==0) break;

            for (int i = wordlen-1; i >= 0; i--){
                if(word[i] == ranges[rangesLen-1].max){
                    word[i] = ranges[0].min;
                }
                else{
                    int k;
                    for (k = 0; k < rangesLen-1; k++){
                        if(word[i] == ranges[k].max){
                            word[i] = ranges[k+1].min;
                            break;
                        }
                    }
                    if(k==rangesLen-1){
                        word[i]++;
                        break;
                    }
                    // printf("-> %d\n",k);
                }
            }
        }
        if(infinity) j=0;
    }

    free(word);
    free(lastword);

    return 0;
}

bool dictWordCrack(Password* password, char* dictWord, HASH_TYPES hashType){
    char* test = digestFactory(dictWord,password->salt,hashType);
    if(strcmp(test,password->hash)==0){
        free(test);
        password->password = calloc(sizeof(char),strlen(dictWord)+1);
        strcpy(password->password,dictWord);
        return true;
    }
    return false;
}

bool singleCrack(Password* password, HASH_TYPES hashType){
    return dictWordCrack(password,password->username,hashType);
}

char* incrementalNextWord(char* word, Range* ranges,int rangesLen){
    int wordlen = strlen(word);

    bool isLast = true;
    for (int i = 0; i < wordlen; i++){
        if(word[i] != ranges[rangesLen-1].max) isLast = false;
    }

    if(isLast) return NULL;

    int k;
    for (k = 0; k < rangesLen-1; k++){
        if(word[wordlen-1] == ranges[k].max){
            word[wordlen-1] = ranges[k+1].min;
            return word;
        }
    }



    if(word[wordlen-1] == ranges[rangesLen-1].max){
        for (int i = wordlen-1; i >= 0; i--){
            if(word[i] == ranges[rangesLen-1].max){
                word[i] = ranges[0].min;
            }
            else {
                word[i]++;
                return word;
            }
        }
    }

    if(k==rangesLen-1){
        word[wordlen-1]++;
        return word;
    }
}