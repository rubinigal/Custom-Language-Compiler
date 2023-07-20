#include "header.h"
#include "assembler.h"

int main(int argc, char *argv[]) 
{
    int i = 1, num_label;
    

    if (argc < 2) /* error if the program activated without file names */
    {
        fprintf(stderr,"\nYou did not proveide file names\n");
        exit(1);
    }


    while (--argc)
    {
        num_label = demacro(argv[i]); /* we send the name of the file amd it will make a .am file, the demacro function also counts and return the number of labels */

        if (num_label != -1) /* something was wrong with the demacro function, that means we cant make properly the files so no need to continue to next file if any */
        {
            phaseOne(num_label, argv[i]); /* we send the number of labels and start phase one, phase two will be called from phase one */
        }

        i++;
    }
    
    return 0;
}




