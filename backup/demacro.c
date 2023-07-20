#include "header.h"
#include "demacro.h"


int demacro(const char file_name[])
{
    FILE *after_macro = NULL, *before_macro = NULL;                                                  /* two files, before and after the macro process */
    int bool_macro_line = 0, num_label = 0, len;                                                     /* used to check if we are inside a macro, we will return the number of labels*/
    char line_code[MAX_LINE_CODE],line_code_copy[MAX_LINE_CODE];                                     /* will store the line of code while the other will be used to copy and change the line */
    char *portion_line_code;                                                                         /* will be used for strtok */
    char macro_name[MAX_LINE_CODE];                                                                  /* will save the macro name */
    char memory_table[MAX_MEMORY][MAX_LINE_CODE];                                                    /* will be used to store the code lines and then used to print to output file */
    int memory_table_index, memory_table_size = 0;                                                   /* used to handle the memory_table matrix */
    char *file_name_extension = (char *)malloc((strlen(file_name) + 4) * sizeof(char));              /* we make space for the name of the file + its extension */
    int macro_tabel_size = 1;                                                                        /* used in malloc and realloc for our macro table size, */
    Macro* macro_table = (Macro*)calloc(macro_tabel_size, sizeof(Macro));                            /* here we will save our macro information and will use realloc if needed */

    if ((file_name_extension == NULL) || (macro_table == NULL)) /*checking if malloc worked*/
    {
        fprintf(stderr,"\nMemory allocation failed\n");
        exit(2);
    }

    strcpy(file_name_extension, file_name);
    strcat(file_name_extension, ".as");
    if ((before_macro = fopen(file_name_extension, "r")) == NULL) /*openning the original file with extension .as and checking if we get an error*/
    {
        fprintf(stderr, "\nsource code file %s could not be opened\n", file_name_extension);
        free(file_name_extension);
        free(macro_table);
        return -1;
    }
    fseek(before_macro,0,SEEK_END); 
    if (ftell(before_macro) == 0)  /* checks if the file is empty */
    {
        fprintf(stderr, "\nsource code file %s is empty\n", file_name_extension);
        free(file_name_extension);
        free(macro_table);
        fclose(before_macro);
        return -1;
    }
    rewind(before_macro);
    
    file_name_extension[strlen(file_name_extension) - 1] = 'm';
    if ((after_macro = fopen(file_name_extension, "w")) == NULL) /* openning the new file with extension .am and checking if we get an error*/
    {
        fprintf(stderr, "\nsource code file %s could not be created\n", file_name_extension);
        free(file_name_extension);
        free(macro_table);
        fclose(before_macro);
        return -1;
    }

    while (fgets(line_code,MAX_LINE_CODE,before_macro) != NULL)
    {
        int macro_location; /* tells where the macro located inside the macro table */

        len = strlen(line_code);
        if ((len == (MAX_LINE_CODE - 1)) && (line_code[len-1] != '\n')) /* code line cant be longer then 80 chars */
        {
            fprintf(stderr,"\nYour line of code is too long\n");
            continue;
        }

        removeExtraWhitespace(line_code);
        strcpy(line_code_copy, line_code);
        if((line_code_copy[0] == '\0') || isspace(line_code_copy[0]) || (line_code_copy[0] == ';')) continue; /* we ignore not relevant lines like comments or empty lines */
        portion_line_code = strtok(line_code_copy, " "); /* we get the first word in the code line */

        if (portion_line_code[(strlen(portion_line_code) - 1)] == ':') num_label++; /* counts labels for next step */
        
        if (strcmp(portion_line_code,"mcr") == 0) /* new macro to add to the macro table */
        {
            portion_line_code = strtok(NULL, " "); /* getting the macro name */
            if (portion_line_code == NULL) /* missing macro name so we will skip code lines till after we get to endmcr */
            {
                fprintf(stderr,"\nMissing macro name\n"); 
                while (strcmp(portion_line_code,"endmcr") != 0) 
                {
                    fgets(line_code,MAX_LINE_CODE,before_macro);
                    portion_line_code = strtok(line_code, " ");
                }
                continue;
            }

            strcpy(macro_name,portion_line_code); /* saving the macro name cause we call for strtok again and we will lose it */
            portion_line_code = strtok(NULL, " "); /* getting the third word if any */
            if ((portion_line_code != NULL) || (0 <= isSavedWord(macro_name)) || (0 <= findMacro(macro_table, macro_tabel_size, macro_name))) /* something wrong with the macro name, too many names, saved word or already taken */
            {
                fprintf(stderr,"\nSomething wrong with the macro name implementation\n");
                while (strcmp(portion_line_code,"endmcr") != 0) /* wrong macro name so we will skip code lines till after we get to endmcr */
                {
                    fgets(line_code,MAX_LINE_CODE,before_macro);
                    portion_line_code = strtok(line_code, " ");
                }
                continue;
            }

            newMacro(macro_table, &macro_tabel_size, macro_name); /* creating a new macro */
            bool_macro_line = 1;
            macro_table = (Macro*)realloc(macro_table, macro_tabel_size*sizeof(Macro)); /* making space for the next macro */

            if (macro_table == NULL) /*checking if realloc worked*/
            {
                fprintf(stderr,"\nMemory allocation failed\n\n");
                exit(2);
            }

            macro_table[macro_tabel_size - 1].num_lines = 0; /* need to reset the number of lines cause realloc does not */
        }
        else if (strcmp(portion_line_code,"endmcr") == 0) /* end of current macro */
        {
            if((portion_line_code = strtok(NULL, " ")) != NULL) fprintf(stderr,"\nWarning: after endmcr all data will be lost on the same line\n"); /* after endmcr there should be only white space */
            bool_macro_line = 0;
        } 
        else if (bool_macro_line != 0) /* a macro line to add */
        {
            addMacro(macro_table, macro_tabel_size - 2, line_code);    
        }
        /* chekcs if the line starts with a macro name that we need to replace and if not the second or checks for if the first word was a label but second word is a macro name */
        else if ((0 <= (macro_location = findMacro(macro_table, macro_tabel_size, portion_line_code))) || (0 <= (macro_location = findMacro(macro_table, macro_tabel_size, portion_line_code = strtok(line_code_copy, " "))))) 
        {
            int i = 0;
            strcpy(line_code_copy, line_code);
            portion_line_code = strtok(line_code_copy, " ");

            if (portion_line_code[(strlen(portion_line_code) - 1)] == ':') /* there is a label before the macro name so we need to add that label first before the unpack the macro */
            {
                char temp[MAX_LINE_CODE]; /* cant use portion_line_code with strcat so i need a temp string (to be more clear i can use it but it will ruin the string line_code_copy) */
                strcpy(temp, portion_line_code);
                strcat(temp, " ");
                strcat(temp, macro_table[macro_location].code[i]);
                strcpy(memory_table[memory_table_size], temp);               
                memory_table_size++;
                i++;
            }
            
            for ( ; i < macro_table[macro_location].num_lines; i++) /* adds the macro lines to the memory table */
            {

                if (memory_table_size < MAX_MEMORY) /* max memory code is limited and we cant go above it */
                {
                    strcpy(memory_table[memory_table_size], macro_table[macro_location].code[i]);
                    memory_table_size++;
                }
                else
                {
                    fprintf(stderr,"\nYour code is too long\n");
                    break;
                }   
            }     
        }
        else /* regular line */
        {
            if (memory_table_size < MAX_MEMORY) /* max memory code is limited and we cant go above it */
            {
                strcpy(memory_table[memory_table_size], line_code);
                memory_table_size++;
            }
            else
            {
                fprintf(stderr,"\nYour code is too long\n");
                break;
            }   
        }    
    }

    for (memory_table_index = 0; memory_table_index < memory_table_size - 1; memory_table_index++) /* filling up the file */
    {
        fputs(memory_table[memory_table_index], after_macro);
        fputc('\n', after_macro);
    }
    fputs(memory_table[memory_table_index], after_macro); /* we dont want the end of file to be new line */

    free(file_name_extension);
    free(macro_table);
    fclose(before_macro);
    fclose(after_macro);
    return num_label;
}

int findMacro(Macro* macro_table,int macro_table_size, char macro_name[]) /* finds the macro inside the macro table, -1 if its not in the table */
{
    int i;

    if(macro_name == NULL) return -1;
    for (i = 0; i < macro_table_size; i++)
    {
        if (strcmp(macro_table[i].name,macro_name) == 0) return i; 
    }

    return -1;
}

void newMacro(Macro* macro_table, int* macro_table_size, char macro_name[]) /* add the new macro name to the macro table */
{
    strcpy(macro_table[*macro_table_size - 1].name, macro_name);
    (*macro_table_size)++;
}

void addMacro(Macro* macro_table, int macro_table_index, char line_code[]) /* adds a line of code for that macro and updating the lines number */
{
    strcpy(macro_table[macro_table_index].code[macro_table[macro_table_index].num_lines], line_code);
    macro_table[macro_table_index].num_lines++;
}