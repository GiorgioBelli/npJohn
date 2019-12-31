typedef enum HASH_TYPES{
    SHA256_t,
    SHA512_t,
    CRYPT_t,
    NOPASSWORD_t,
    NONETYPE_t
} HASH_TYPES;


#ifndef STDBOOL_H
    #include <stdbool.h>
#endif

#ifndef STDIO_H
    #include <stdio.h>
#endif

#ifndef STDLIB_H
    #include <stdlib.h>
#endif

#ifndef STRING_H
    #include <string.h>
#endif

#ifndef PASSWORD_H
    #include "../password/password.h"
#endif

#ifndef MD5_H
    #include <openssl/md5.h>
#endif

#ifndef SHA_H
    #include <openssl/sha.h>
#endif

#ifndef CRYPT_H
    #include <crypt.h>
#endif

#define DIGEST_H

#define _XOPEN_SOURCE
#define CRYPT_DIGEST_LENGTH 13

bool setDebugPrints(bool enabled);
void print(char* format,char* string); //printa solo se abilitato il flag di print
char* prepSaltedKey(char* key, char* salt);

char* md5(char* plaintext,char* hash, char* salt);
char* sha256(char* plaintext,char* hash, char* salt);
char* sha512(char* plaintext,char* hash, char* salt);
char* unixCrypt(char* key,char* hash, char* salt);
char* digestFactory(char* key, char* salt, HASH_TYPES hashType, char* hash);

int getDigestLen(HASH_TYPES hashType);
HASH_TYPES getTypeHash(Password* pwd);

#include "digest.c"
