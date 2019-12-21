#include "io.h"
#include <unistd.h>

int main(int argc, char *argv[]){

char* output_file_path;
int opt; 

while((opt = getopt(argc, argv, ":o:")) != -1)  
{  
    switch(opt)  
    {  
        case 'o':  
            output_file_path = optarg; 
            break;  
        case ':':  
            if (opt == 'o') {
                printf("The option -o needs an output file as an argument.\n");  
            }
            break;  
        case '?':  
            printf("Unknown option: %c\n", optopt); 
            break;  
    }  
}  


write_final_output("Ciaoooo", output_file_path);

return 0;
}
