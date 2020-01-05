#define PASSWORD_H 

typedef struct Password{
    char* username;
    char* hash;
    char* password;
    char* salt;
    HASH_TYPES hashType;
} Password;