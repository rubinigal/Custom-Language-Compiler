#include "tables.h"

void printBinary(int num); /* prints number as binary including fowarrding zeros */
int isSavedWord(char str[]);
int findSymbol(const Symbol* symbol_table, const int symbol_table_size, char symbol_name[]);
void newSymbol(Symbol* symbol_table, int* symbol_table_size, char symbol_name[], const int address, const char type);
void fileMaker(Word* memory_table, int IC, int DC, Symbol* entry_table, int entry_table_size, Symbol* extern_table, int extern_table_size, const char file_name[]); /* the function that creats the files */
