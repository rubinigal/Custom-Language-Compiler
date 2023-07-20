#ifndef TABLES_H

typedef struct Symbol /* we define the struct of the macro we will get */
{
    char name[MAX_LABEL_SIZE];
    int address;
    char type;
} Symbol;

typedef struct Bits
{
    unsigned int ERA:2;
    unsigned int target:2;
    unsigned int source:2;
    unsigned int opcode:4;
    unsigned int par2:2;
    unsigned int par1:2;
} Bits;

typedef struct Register
{
    unsigned int ERA:2;
    unsigned int target:6;
    unsigned int source:6;
} Register;

typedef union Word
{
    int number;
    Bits bits;
    Register reg;
} Word;

int isSavedWord(char str[]);
int isLegalNum(char str[], int* num ,int* num_error, int num_line);

#endif