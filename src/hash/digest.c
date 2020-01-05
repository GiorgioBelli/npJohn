bool debugPrintFlag = false;

bool setDebugPrints(bool enabled){
    debugPrintFlag = enabled;
    return debugPrintFlag;
}

//singleString print
void print(char* format,char* string){
    if(debugPrintFlag) printf(format,string);
    return;
}

char* prepSaltedKey(char* key, char* salt){
    
    char* saltedKey = (char*) calloc(sizeof(char),strlen(key)+strlen(salt));

    strcat(saltedKey,salt);
    strcat(saltedKey,key);

    print("salted key: %s\n",saltedKey);

    return saltedKey; //remenber to free this pointer


}

char* md5(char* plaintext,char* hash, char* salt) {
    unsigned char digest[MD5_DIGEST_LENGTH];

    char* salted = prepSaltedKey(plaintext,salt);
 
    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx, salted, strlen(salted));
    MD5_Final(digest, &ctx);
 
    free(salted); 

    // char* mdString = (char*) calloc(sizeof(char),MD5_DIGEST_LENGTH*2+1);

    for (int i = 0; i < MD5_DIGEST_LENGTH; i++)
        sprintf(&hash[i*2], "%02x", (unsigned int)digest[i]);

    hash[MD5_DIGEST_LENGTH*2] = '\0';

    print("MD5 digest: %s\n",hash);

    return hash;
}

char* sha512(char* plaintext,char* hash, char* salt){
    unsigned char digest[SHA_DIGEST_LENGTH];
    char* salted = prepSaltedKey(plaintext,salt);
    
    SHA_CTX ctx;
    SHA512_Init(&ctx);
    SHA512_Update(&ctx, salted, strlen(salted));
    SHA512_Final(digest, &ctx);

    free(salted); 
    
    // char* mdString = (char*) calloc(sizeof(char),SHA_DIGEST_LENGTH*2+1);

    for(int i = 0; i < SHA_DIGEST_LENGTH; i++)
        sprintf(&hash[i*2], "%02x", (unsigned int)digest[i]);

    hash[SHA_DIGEST_LENGTH*2] = '\0';

    print("SHA512 digest: %s\n",hash);
 
    return hash; //remenber to free this pointer
}

char* sha256(char* plaintext,char* hash, char* salt) {
    unsigned char digest[SHA256_DIGEST_LENGTH];
    char* salted = prepSaltedKey(plaintext,salt);
 
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, salted, strlen(salted));
    SHA256_Final(digest, &ctx);
    
    free(salted); 
    
    // char* mdString = (char*) calloc(sizeof(char),SHA256_DIGEST_LENGTH*2+1);

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++){
        sprintf(&hash[i*2], "%02x", (unsigned int)digest[i]);

    }

    hash[SHA256_DIGEST_LENGTH*2] = '\0';

    print("SHA256 digest: %s\n",hash);

 
    return hash;
}

char* unixCrypt(char* key,char* hash, char* salt){
    // char* mdString = (char*) calloc(sizeof(char),CRYPT_DIGEST_LENGTH+1);
    
    strncpy(hash,crypt(key,salt),CRYPT_DIGEST_LENGTH);

    print("crypt digest: %s\n",hash);

    return hash;
}

char* digestFactory(char* key, char* salt, HASH_TYPES hashType, char* hash){
    if(hashType == SHA512_t) return sha512(key,hash,salt);
    if(hashType == SHA256_t) return sha256(key,hash,salt);
    if(hashType == CRYPT_t) return unixCrypt(key,hash,salt);
    else return NULL;
}

int getDigestLen(HASH_TYPES hashType){
    if(hashType == SHA512_t) return SHA_DIGEST_LENGTH;
    if(hashType == SHA256_t) return SHA256_DIGEST_LENGTH;
    if(hashType == CRYPT_t) return CRYPT_DIGEST_LENGTH;
    else return 0;
}

HASH_TYPES getTypeHash(int id){
    HASH_TYPES hashType = NONETYPE_t ;
    
    if( id == 5 ){
        hashType = SHA256_t;
    }else if( id == 6 ){
        hashType = SHA512_t;
    }

    return hashType;
}
