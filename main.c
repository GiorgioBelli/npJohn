#include "hash/digest.h"

int main(int argc, char const *argv[])
{
    setDebugPrints(1);

    pass obj;
    obj.user = "user";
    obj.cryptHash = "mmeiners:e7hvcqLV0YUmQ:53212:532:Cpr E 532 Student:/home/issl/532/mmeiners:/bin/tcsh";

    free(digestFactory("ciao","ab", getHashType(obj)));

    return 0;
}