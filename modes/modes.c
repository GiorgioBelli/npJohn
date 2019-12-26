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

bool dictWordCrack(Password* password, char* dictWord, HASH_TYPES hashType,RULES ruleType, Range ranges[],int rangesLen,int incrementalLen){

    //checking for necessary not null parameters
    if(password==NULL || dictWord==NULL || hashType==NONETYPE_t) return false;
    
    int dictWordLen = strlen(dictWord);

    // checking for crypt word maxsize
    if(hashType == CRYPT_t && strlen(dictWord)>CRYPT_t_MAXSIZE) {
        password->password = TOO_LONG_WORD;
        return true;
    } 

    // preparing buffer for hashed value (in hex)
    char* test = calloc(sizeof(char),getDigestLen(getTypeHash(password))*2+1);

    //generation of the dictWord and testing with password hash
    digestFactory(dictWord,password->salt,hashType,test);
    if(strcmp(test,password->hash)==0){
        free(test);
        password->password = calloc(sizeof(char),dictWordLen+1);
        strcpy(password->password,dictWord);
        return true;
    }

    //check if rules are required and all parameters are passed
    if( ruleType == NO_RULE || 
        rangesLen==0 || 
        (ruleType==CRYPT_t && dictWordLen+incrementalLen>CRYPT_t_MAXSIZE)){
            return false;
        }

    char* incrementalWord = calloc(sizeof(char),incrementalLen+1);

    for (int i = 0; i < incrementalLen; i++) incrementalWord[i]='0';
    incrementalWord[incrementalLen] = '\0';
    
    char* concatWord = calloc(sizeof(char),dictWordLen+incrementalLen+1);
    strcpy(concatWord,dictWord);

    //incremental word testing
    int counter = 0;

    
    do{
        counter++;
        concatWord[dictWordLen] = '\0';
        strcat(concatWord,incrementalWord);

        digestFactory(concatWord,password->salt,hashType, test);
        
        // printf("\n%s\n%s\n%s\n",concatWord,test,password->hash);
        // fflush(stdout);

        if(strcmp(test,password->hash)==0){
            free(test);
            free(incrementalWord);
            free(concatWord);

            password->password = calloc(sizeof(char),dictWordLen+incrementalLen+1);
            strcpy(password->password,concatWord);
            return true;
        }

    }while(incrementalNextWord(incrementalWord,ranges,rangesLen)!=NULL);

    free(incrementalWord);
    free(concatWord);

    return false;
}

bool singleCrack(Password* password, HASH_TYPES hashType){
    return dictWordCrack(password,password->username,hashType,NO_RULE,NULL,0,0);
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

char* get_nth_word(Range[] ranges){

    // TODO retrieve the nth word inside the universe of words generates with that range

    return NULL;
}