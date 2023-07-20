#include "tables.h"

int isSavedWord(char str[]);
int isNumOrAlpha(char str[]);
int isLegalNum(char str[], int* num ,int* num_error, int num_line);                                                    /* check if string a number, returns false/true (0/1) and saves the number in num */
int findSymbol(const Symbol* symbol_table, const int symbol_table_size, char symbol_name[]);                           /* finds the symbol inside the symbol table, -1 if its not in the table */
void newSymbol(Symbol* symbol_table, int* symbol_table_size, char symbol_name[], const int address, const char type);  /* add the new symbol name to the symbol table */
int data(Word* memory_table, char data_line[], int* DC, char type, int num_line);                                      /* gets .data and .string and add the data to the memory table */
int instruction(Word* memory_table,char name_comm[], char code_line[], int* IC, int num_line);                         /* gets an instruction line cheking if its grammar correct and adds the word to the memory table */
void rearrangeTable(Word* memory_table, int IC, int DC);                                                               /* swaps the order of elements in memory_table, all my data is at the end and i need to connect it to the end of IC */
void fixSymbols(Symbol* symbol_table, int symbol_table_size, int IC);                                                  /* this function fixes the right address for data labels */
int fixEntry(Symbol* symbol_table, int symbol_table_size, Symbol* entry_table, int entry_table_size);                  /* updates the adress for entry labes */
void phaseTwo(Word* memory_table, int IC, int DC, Symbol* symbol_table, int symbol_table_size, Symbol* entry_table, int entry_table_size, Symbol* extern_table, int extern_table_size, FILE* am, const char file_name[], int* num_error);
