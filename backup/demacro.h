
typedef struct Macro /* we define the struct of the macro we will get */
{
    char name[MAX_LINE_CODE];
    char code[MAX_LINE_CODE / 4][MAX_LINE_CODE];
    int  num_lines;   
} Macro;

void removeExtraWhitespace(char *str);
int  isSavedWord(char str[]);
int  findMacro(Macro* macro_table, int macro_table_size, char macro_name[]); /* finds the macro inside the macro table, -1 if its not in the table */
void newMacro(Macro* macro_table, int* macro_table_size, char macro_name[]); /* add the new macro name to the macro table */
void addMacro(Macro* macro_table, int macro_table_index, char line_code[]);  /* adds a line of code for that macro and updating the lines number */