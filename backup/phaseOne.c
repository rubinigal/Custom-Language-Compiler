#include "header.h"
#include "phaseOne.h"


void phaseOne(int num_label, const char file_name[])
{
    int IC = 0, DC = 0, num_error = 0, num_line, len;                                     /* len will be used firstly to reset the names in the symbol table and then it will be used as length of the line code untill the arguments start */
    char line_code[MAX_LINE_CODE],line_code_copy[MAX_LINE_CODE];                          /* will store the line of code while the other will be used to copy and change the line */
    char *portion_line_code;                                                              /* will be used for strtok */
    FILE *am;                                                                             /* am is the after macro file */
    Word memory_table[MAX_MEMORY];                                                       /* will be used to store the code lines */
    char *file_name_extension = (char *)malloc((strlen(file_name) + 4) * sizeof(char));   /* we make space for the name of the file + its extension */
    int entry_table_size = 1, extern_table_size = 1, symbol_table_size = 1;               /* will be used to know the size of table and where to put the next object in the table*/
    Symbol* symbol_table = (Symbol*)malloc(num_label*sizeof(Symbol));                     /* here we will save our symbols information */
    Symbol* entry_table = (Symbol*)malloc(entry_table_size*sizeof(Symbol));               /* here we record the .entry objects */
    Symbol* extern_table = (Symbol*)malloc(extern_table_size*sizeof(Symbol));             /* here we record the .extern objects */

    if ((symbol_table == NULL) || (entry_table == NULL) || (extern_table == NULL) || (file_name_extension == NULL)) /*checking if malloc worked*/
    {
        fprintf(stderr,"\nMemory allocation failed\n");
        exit(2);
    }

    strcpy(file_name_extension, file_name);
    strcat(file_name_extension, ".am");
    if ((am = fopen(file_name_extension, "r")) == NULL) /* openning the file after macro .am and checking if we get an error*/
    {
        fprintf(stderr, "\nsource code file %s could not be opened\n", file_name_extension);
        free(file_name_extension);
        free(symbol_table);
        free(entry_table);
        free(extern_table);
        return;
    }

    for (num_line = 1; fgets(line_code,MAX_LINE_CODE,am) != NULL; num_line++) /* we starts to read .am file line by line */
    {
        len = strlen(line_code);
        if (line_code[len - 1] == '\n') line_code[len - 1] = '\0'; /* removes '\n' */
        strcpy(line_code_copy, line_code);
        portion_line_code = strtok(line_code_copy, " "); /* we get the first word in the code line */
        len = strlen(portion_line_code);

        if (portion_line_code[(len - 1)] == ':') /* if the code line starts with a label */
        {
            portion_line_code[len - 1] = '\0'; /* remove the : so we can compare the strings */
            
            if (len >= MAX_LABEL_SIZE) /* max size of label is 31 */
            {
                fprintf(stderr,"\nLine: %d; Your label name is too long\n", num_line);
                num_error++;
                continue;
            }
            if (!isalpha(portion_line_code[0])) /* label needs to start with alphabetic character */
            {
                fprintf(stderr,"\nLine: %d; The label name needs to start with alphabetic character\n", num_line);
                num_error++;
                continue;
            }
            if (!isNumOrAlpha(portion_line_code)) /* label name is illegal  */
            {
                fprintf(stderr,"\nLine: %d; The label name needs to contain only alphabetic character and numbers\n", num_line);
                num_error++;
                continue;
            }
            if (isSavedWord(portion_line_code) != -1) /* label is a saved word */
            {
                fprintf(stderr,"\nLine: %d; You can not use a saved word for a label name\n", num_line);
                num_error++;
                continue;
            }
            if (findSymbol(symbol_table, symbol_table_size, portion_line_code) != -1) /* checks if we are trying to add the same label twice */
            {
                fprintf(stderr,"\nLine: %d; You can not use the same label name more then once\n", num_line);
                num_error++;
                continue;
            }
            if ((portion_line_code = strtok(NULL, " ")) == NULL) /* empty line after label */
            {
                fprintf(stderr,"\nLine: %d; You can not use label for an empty line\n", num_line);
                num_error++;
                continue;
            }
            /* we first look for a label for data, then we check that its not a label for entry or extern, line_code_copy still points to the first word aka the label name */
            if ((strcmp(portion_line_code,".data") == 0) || (strcmp(portion_line_code,".string") == 0))
            {
                newSymbol(symbol_table, &symbol_table_size, line_code_copy, DC, 'D'); /* D stands for data */
            }
            else if (strcmp(portion_line_code,".entry") && strcmp(portion_line_code,".extern"))
            {
                newSymbol(symbol_table, &symbol_table_size, line_code_copy, IC, 'I'); /* I stands for instruction */
            }
            else  /* in this code we ignore the labels that are before .entry or .extern directives they mean nothting and thats why we only warning about it */
            {
                fprintf(stderr,"\nLine: %d; Warning: a label before .entry or .extern directives is ignored\n", num_line);
            }

            len += (strlen(portion_line_code) + 1); /* from here we will use len as a pointer to where the parameters start in line_code_copy and also strtok didnt changed that part */ 
        }

        /* even if we entered the first if statment or not, portion_line_code will point to the command name or directive */
        if (portion_line_code[0] == '.') /* a call for a directive */
        {
            if (strcmp(portion_line_code,".data") && strcmp(portion_line_code,".string") && strcmp(portion_line_code,".entry") && strcmp(portion_line_code,".extern"))
            {
                fprintf(stderr,"\nLine: %d; No directive with the name: %s exists\n", num_line, portion_line_code);
                num_error++;
                continue;
            } 
            if ((len + 1) > strlen(line_code)) /* No variables found after directive, len+1 will point to the start of the word after directive name */
            {
                fprintf(stderr,"\nLine: %d; No variables found after directive\n", num_line);
                num_error++;
                continue;
            }

            if (strcmp(portion_line_code,".data") == 0)
            {
                num_error += data(memory_table, &line_code_copy[len + 1], &DC, 'i', num_line); /* i stands for int */
            }
            else if (strcmp(portion_line_code,".string") == 0)
            {
                num_error += data(memory_table, &line_code_copy[len + 1], &DC, 's', num_line); /* s stands for string */
            }
            else if (strcmp(portion_line_code,".entry") == 0)
            {
                portion_line_code = strtok(NULL, " "); /* gets the label name, strtok wont return NULL cause we checked before */
                if (strtok(NULL, " ") != NULL) /* entry has more then one variable */
                {
                    fprintf(stderr,"\nLine: %d; Can not define more then one label name\n", num_line);
                    num_error++;
                    continue;
                }
                
                if ((findSymbol(entry_table, entry_table_size, portion_line_code) == -1) && (findSymbol(extern_table, extern_table_size, portion_line_code) == -1))
                {
                    newSymbol(entry_table, &entry_table_size, portion_line_code, 0, 'e'); /* address is 0 cause we dont know it yet */
                    
                    entry_table = (Symbol*)realloc(entry_table, entry_table_size*sizeof(Symbol)); /* making space for the next symbol */
                    if (entry_table == NULL) /*checking if realloc worked*/
                    {
                        fprintf(stderr,"\nMemory allocation failed\n\n");
                        exit(2);
                    }
                }
                else /* this symbol name already exists */
                {
                    fprintf(stderr,"\nLine: %d; Can not define the same label twice\n", num_line);
                    num_error++;
                }
            }
            else /* last option is .extern */
            {
                /* this table will be used to check that if labes arent in entry maybe they in extern and if not its an error, will be checked in phaseTwo */
                portion_line_code = strtok(NULL, " "); /* gets the label name, strtok wont return NULL cause we checked before */
                if (strtok(NULL, " ") != NULL) /* extern has only one variable */
                {
                    fprintf(stderr,"\nLine: %d; Can not define more then one label name\n", num_line);
                    num_error++;
                    continue;
                }

                if ((findSymbol(entry_table, entry_table_size, portion_line_code) == -1) && (findSymbol(extern_table, extern_table_size, portion_line_code) == -1))
                {
                    newSymbol(extern_table, &extern_table_size, portion_line_code, 0, 'e'); /* address is 0 cause we cant not know it */
                    
                    extern_table = (Symbol*)realloc(extern_table, extern_table_size*sizeof(Symbol)); /* making space for the next symbol */
                    if (extern_table == NULL) /*checking if realloc worked*/
                    {
                        fprintf(stderr,"\nMemory allocation failed\n\n");
                        exit(2);
                    }
                }
                else /* this symbol name already exists */
                {
                    fprintf(stderr,"\nLine: %d; Can not define the same label twice\n", num_line);
                    num_error++;
                }
            }
        }
        else /* if its not a directive its a instruction */
        {
            num_error += instruction(memory_table, portion_line_code, &line_code_copy[len + 1], &IC, num_line);
        }
    }

    if ((IC + DC) >= MAX_MEMORY) /* the code has more then 256 words those cannot run on Maman 14 ram */
    {
        fprintf(stderr, "\nsource code file %s is too long for a %d memory machine\n", file_name, MAX_MEMORY);
    }
    
    rearrangeTable(memory_table, IC, DC); /* swaps the order of elements in memory_table, all my data is at the end and i need to connect it to the end of IC */
    fixSymbols(symbol_table, symbol_table_size, IC); /* this function fixes the correct address for data labels (now i know where the data words start) */
    if (!fixEntry(symbol_table, symbol_table_size -1, entry_table, entry_table_size -1)) 
    {
        fprintf(stderr,"\nOne or more of your entry label(s) not exists in your code\n");
        num_error++;
    }
    
    rewind(am);
    phaseTwo(memory_table, IC , DC, symbol_table, symbol_table_size - 1, entry_table, entry_table_size - 1, extern_table, extern_table_size - 1, am, file_name, &num_error);

    free(file_name_extension);
    free(symbol_table);
    free(entry_table);
    free(extern_table);
    fclose(am);
}

