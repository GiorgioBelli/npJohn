#include "hash/digest.h"
#include "modes/modes.h"

int main(int argc, char const *argv[])
{
    /* test digestFactory */
    setDebugPrints(1);
    free(digestFactory("ciao","ab",CRYPT_t));

    /* test incremental */
    Range ranges[] = {{48,57},{65,90},{97,122}};
    if(sizeof(ranges)==0) return 0;
    printf("%ld\n",sizeof(ranges)/sizeof(ranges[0]));
    incremental(1,0,ranges,sizeof(ranges)/sizeof(ranges[0]));

    return 0;
}