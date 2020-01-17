PasswordList* createStruct(char* fileName){
    FILE *fp;
    char str[MAXCHAR];
    int startHash;
    int offsetHash;
    int offsetID = 0;
    int offsetSalt = 0;
    int offsetpsw = 0;
    int count = 1;

    fp = fopen(fileName, "r");
    if (fp == NULL){
        printf("Could not open file %s",fileName);
        return NULL;
    }

    PasswordList* head = NULL;

    while (fgets(str, MAXCHAR, fp) != NULL){
        str[strlen(str)-1] = '\0';
        int len = strlen(str);
        int c = 0, lenData = 0, endName, endId, endSalt;
        Password* obj = (Password*)malloc(sizeof(Password));

        obj->password = NULL;

        //Scorro la riga finche' non trovo la fine del nome utente per poi salvarlo
        for(c; c<len; c++){
            if( str[c] == ':' ){
                break;
            }else{
                lenData++;
            }
        }
        obj->username = (char *)malloc(sizeof(char)*lenData+1);
        strncpy(obj->username, str, lenData);
        obj->username[lenData] = '\0';

        c++;
        endName = c;
        lenData = 0;

        //Se subito dopo i : ho ancora : allora l'utente non ha password
        if( str[c] == ':' ){
            obj->salt = (char *)malloc(sizeof(char)*2);
            obj->salt[0] = '-';
            obj->salt[1] = '\0';

            obj->hash = (char *)malloc(sizeof(char)*2);
            obj->hash[0] = '-';
            obj->hash[1] = '\0';
            
            obj->hashType = NOPASSWORD_t;

        //Se dopo i : invece ho $ allora troverò un id-salt e l'hash
        }else if( str[c] == '$' ){
            
            c++;
            endName = c;
            //Cerco la fine dell'id
            for(c; c<len; c++){
                if( str[c] == '$' ){
                    break;
                }else{
                    lenData++;
                }
            }
            char* idStr = (char *)malloc(sizeof(char)*lenData+1);
            strncpy(idStr, &str[endName], lenData);
            idStr[lenData] = '\0';
            int ID = atoi(idStr);
            free(idStr);
            obj->hashType = getTypeHash(ID);

            lenData = 0;
            c++;
            endId = c;

            for(c; c<len; c++){
                if( str[c] == '$' ){
                    break;
                }else{
                    lenData++;
                }
            }
            obj->salt = (char *)malloc(sizeof(char)*lenData+1);
            strncpy(obj->salt, &str[endId], lenData);
            obj->salt[lenData] = '\0';

            lenData = 0;
            c++;
            endSalt = c;

            for(c; c<len; c++){
                if( str[c] == ':' ){
                    break;
                }else{
                    lenData++;
                }
            }
            obj->hash = (char *)malloc(sizeof(char)*lenData+1);
            strncpy(obj->hash, &str[endSalt], lenData);
            obj->hash[lenData] = '\0';

        //Se dopo i : non ho nemmeno $ allora ho direttamente l'hash realizzato con CRYPT e l'has sono i primi 2 caratteri
        }else{
            for(c; c<len; c++){
                if( str[c] == ':' ){
                    break;
                }else{
                    lenData++;
                }
            }
            obj->salt = (char *)malloc(sizeof(char)*3);
            strncpy(obj->salt, &str[endName], 2);
            obj->salt[2] = '\0';

            obj->hash = (char *)malloc(sizeof(char)*lenData+1);
            strncpy(obj->hash, &str[endName], lenData);
            obj->hash[lenData] = '\0';

            obj->hashType = CRYPT_t;
        }

        PasswordList* node = (struct passwordList *)malloc(sizeof(struct passwordList));
        node->obj = *obj;
        node->next = NULL;
        
        if( head == NULL ){
            head = node;
        }else{
            PasswordList* current = head;
            while( current->next ){
                current = current->next;
            }
            current->next = node;
        }
    }
    fclose(fp);

    return head;
}

DictList* importFileDict(char* fileName){
    FILE *fp;
    char str[MAXCHAR];

    fp = fopen(fileName, "r");
    if (fp == NULL){
        printf("Could not open file %s",fileName);
        return NULL;
    }

    DictList* head = NULL;

    while (fgets(str, MAXCHAR, fp) != NULL){

        DictList* node = (struct dictList *)malloc(sizeof(struct dictList));

        node->word = (char *)malloc(sizeof(char)*strlen(str));
        strncpy(node->word, str, strlen(str)-1);
        node->word[strlen(str)] = '\0';
        node->next = NULL;
        
        if( head == NULL ){
            head = node;
        }else{
            DictList* current = head;
            while( current->next ){
                current = current->next;
            }
            current->next = node;
        }
    }

    return head;
}

void freePass(PasswordList* head){
    PasswordList* current = head;
    while(head){
        
        free(head->obj.hash);
        free(head->obj.username);
        free(head->obj.password);
        free(head->obj.salt);
        
        current = head;
        head = head->next;

        free(current);
    }
}

void freeDict(DictList* head){
    DictList* current = head;
    while(head){
        
        free(head->word);

        current = head;
        head = head->next;

        free(current);
    }
}