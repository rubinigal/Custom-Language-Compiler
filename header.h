#ifndef ASSEMBLER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 

#define MAX_LINE_CODE 82   /* max code line is 80 + 2 for the '\n' and '\0' char */
#define MAX_MEMORY 256     /* max instruction and data lines we can load to the RAM */
#define MEMORY_START 100   /* where the memory address starts */
#define MAX_LABEL_SIZE 31  /* max labe size is 30 + 1 for '\0' char */
#define MAX_NUM_COMM 16    /* we have 16 commands */
#define MAX_SAVED_WORDS 24 /* this is for saved_word array */
#define MAX_INT_NUM 8191   /* 2^13 and 14-bit for negative numbers flag*/

#endif