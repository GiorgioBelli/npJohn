#include "hash/digest.h"

int main(int argc, char const *argv[])
{
    setDebugPrints(1);
    free(digestFactory("ciao","ab",CRYPT_t));
    return 0;
}