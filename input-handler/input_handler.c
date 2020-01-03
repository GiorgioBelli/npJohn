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
        startHash = (int)((char *)strchr(str, ':') - str);
        offsetHash = (int)((char *)strchr(&str[startHash+1], ':') - &str[startHash+1]);

        Password* obj = (Password*)malloc(sizeof(Password));
        obj->password = NULL;

        obj->username = (char *)malloc(sizeof(char)*startHash+1);
        strncpy(obj->username, str, startHash);
        obj->username[startHash+1] = "\0";
        
        if(offsetHash == 0){
            obj->salt = NULL;

            obj->hash = NULL;
            
        }else{

            
            if ((char *)strchr(&str[startHash+2], '$') == NULL){
                offsetpsw = (int)((char *)strchr(&str[startHash+2], ':') - &str[startHash+2]);

                obj->hashType = NULL;

                obj->hash = (char *)malloc(sizeof(char)*offsetpsw+1);
                strncpy(obj->hash, &str[startHash+2], offsetpsw);
                obj->hash[offsetpsw+1] = "\0";

                obj->salt = (char *)malloc(sizeof(char)*3);
                strncpy(obj->salt, &str[startHash+2], 2);
                obj->salt[3] = "\0";
                
            }else{
                offsetID = (int)((char *)strchr(&str[startHash+2], '$') - &str[startHash+2]);
                offsetSalt = (int)((char *)strchr(&str[startHash+offsetID+3], '$') - &str[startHash+offsetID+3]);
                offsetpsw = (int)((char *)strchr(&str[startHash+offsetID+offsetSalt+4], ':') - &str[startHash+offsetID+offsetSalt+4]);
            
                obj->hashType = (char *)malloc(sizeof(char)*offsetID+1);
                strncpy(obj->hashType, &str[startHash+2], offsetID);
                obj->hashType[offsetID+1] = "\0";

                obj->salt = (char *)malloc(sizeof(char)*offsetSalt+1);
                strncpy(obj->salt, &str[startHash+offsetID+3], offsetSalt);
                obj->salt[offsetSalt+1] = "\0";

                obj->hash = (char *)malloc(sizeof(char)*offsetpsw+1);
                strncpy(obj->hash, &str[startHash+offsetID+offsetSalt+4], offsetpsw);
                obj->hash[offsetpsw+1] = "\0";
            }
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