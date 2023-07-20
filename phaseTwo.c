#include "header.h"
#include "phaseTwo.h"

void phaseTwo(Word* memory_table, int IC, int DC, Symbol* symbol_table, int symbol_table_size, Symbol* entry_table, int entry_table_size, Symbol* extern_table, int extern_table_size, FILE* am, const char file_name[], int* num_error)
{
    int len, num_line, extern_table_file_size = 1, i, ICC = 0, index; /* ICC is IC counter */
    char line_code[MAX_LINE_CODE],line_code_copy[MAX_LINE_CODE];
    char *portion_line_code, *p;
    Symbol* extern_table_file = (Symbol*)malloc(extern_table_file_size*sizeof(Symbol)); /* will be used to save extern labels adress inside of code */

    if (extern_table_file == NULL) /*checking if malloc worked*/
    {
        fprintf(stderr,"\nMemory allocation failed\n");
        exit(2);
    }

    for (num_line = 1; fgets(line_code,MAX_LINE_CODE,am) != NULL; num_line++) /* we starts to read .am file line by line */
    {
        line_code[strlen(line_code) - 1] = '\0'; /* removes '\n' */
        strcpy(line_code_copy, line_code);
        portion_line_code = strtok(line_code_copy, " "); /* we get the first word in the code line */
        len = strlen(portion_line_code);

        if (portion_line_code[len - 1] == ':') portion_line_code = strtok(NULL, " "); /* we started with label and we ignore it */
        if (*portion_line_code == '.') continue; /* .data or .string does not have labels to change address and .entry/.extern we already taken care of in phaseOne */
        
        if ((strcmp(portion_line_code, "jmp") == 0) || (strcmp(portion_line_code, "bne") == 0) || (strcmp(portion_line_code, "jsr") == 0)) /* one of the jump commands */
        {
            portion_line_code = strtok(NULL, " "); /* first word is command name so we skip it */
            p = strchr(portion_line_code, '(');
            if (p != NULL) *p = ',';
            p = strchr(portion_line_code, ')');
            if (p != NULL) *p = '\0';
            portion_line_code = strtok(portion_line_code, ",");

            if ((*portion_line_code == '#') || (isSavedWord(portion_line_code) != -1))
            {
                fprintf(stderr,"\nLine: %d; Not a legal label name\n", num_line);
                (*num_error)++;
                continue;
            }   
        }
        else 
        {
            portion_line_code = strtok(NULL, ","); /* first word is command name so we skip it */
            if (portion_line_code == NULL) continue; /* commands with no variables */
        }

        while (portion_line_code != NULL)
        {
            if (*portion_line_code == ' ') portion_line_code++;

            if ((*portion_line_code != '#') && (isSavedWord(portion_line_code) < 16)) /* the variable is a label */
            {
                if ((findSymbol(symbol_table, symbol_table_size, portion_line_code) == -1) && (findSymbol(extern_table, extern_table_size, portion_line_code) == -1))
                {
                    fprintf(stderr,"\nLine: %d; The label %s not in your code and not an extern label\n", num_line, portion_line_code);
                    (*num_error)++;
                    break;
                }
                else
                {
                    for ( i = ICC; i < IC; i++) /* finds the next ERA = 2 aka a label word */
                    {
                        if (memory_table[i].bits.ERA == 2) break;                   
                    }
                    if (i == IC) break;
                    ICC = i + 1; /* we start from the next word in memory table for the next label */

                    index = findSymbol(symbol_table, symbol_table_size, portion_line_code); /* label inside code */
                    if (index >= 0)
                    {
                        memory_table[i].number = (symbol_table[index].address << 2);
                        memory_table[i].bits.ERA = 2;

                    }
                    else /* the label is extern */
                    {
                        memory_table[i].bits.ERA = 1;

                        newSymbol(extern_table_file, &extern_table_file_size, portion_line_code, i, 'e'); /* updating the extern table */
                    
                        extern_table_file = (Symbol*)realloc(extern_table_file, extern_table_file_size*sizeof(Symbol)); /* making space for the next symbol */
                        if (extern_table_file == NULL) /*checking if realloc worked*/
                        {
                            fprintf(stderr,"\nMemory allocation failed\n\n");
                            exit(2);
                        }
                    }
                    
                }     
            }
            
            portion_line_code = strtok(NULL, ",");
        }        
    }
    
    if (*num_error > 0)
    {
       fprintf(stderr,"\nThe file \"%s\" has %d error(s), could not make output files!\n", file_name, *num_error);
    }
    else
    {
        fileMaker(memory_table, IC , DC, entry_table, entry_table_size, extern_table_file, extern_table_file_size -1, file_name);
    }
    
    free(extern_table_file);
}
