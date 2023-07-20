#include "header.h"
#include "tables.h"

int findSymbol(const Symbol* symbol_table, const int symbol_table_size, char symbol_name[])  /* finds the symbol inside the symbol table, -1 if its not in the table */
{
    int i, len;

    if(symbol_name == NULL) return -1;
    len = strlen(symbol_name);

    if (*symbol_name == ' ') symbol_name++;
    if (symbol_name[len - 1] == ' ') symbol_name[len - 1] = '\0';

    for (i = 0; i < symbol_table_size; i++)
    {
        if (strcmp(symbol_table[i].name,symbol_name) == 0) return i; 
    }

    return -1;
}

void newSymbol(Symbol* symbol_table, int* symbol_table_size, char symbol_name[], const int address, const char type)  /* add the new symbol name to the symbol table */
{
    int len;

    len = strlen(symbol_name);

    if (*symbol_name == ' ') symbol_name++;
    if (symbol_name[len - 1] == ' ') symbol_name[len - 1] = '\0';

    strcpy(symbol_table[*symbol_table_size - 1].name, symbol_name);
    symbol_table[*symbol_table_size - 1].address = address + MEMORY_START;
    symbol_table[*symbol_table_size - 1].type = type;
    (*symbol_table_size)++;
}

int data(Word* memory_table, char data_line[], int* DC, char type, int num_line)  /* gets .data and .string and add the data to the memory table */
{
    /* we are using the same memory_table for data and for instructions, instructions will go to the beginning and data from the end */
    /* we know the max memory lines are 256 so as long as both not pointing to the same line we can add more lines to the table */
    /* at the end we will run a loop to change the line order of data line from end table to the end of instruction lines */
    /* this functions is only for data line, instruction lines will go to a diffrent function */
    int num_error = 0, num, len;
    char *p;

    if (type == 'i') /* i stands for int */
    {
        if (strchr(data_line,'.') != NULL)
        {
            fprintf(stderr,"\nLine: %d; You can not use float number(s)\n", num_line);
            num_error++;
        }
        else if (strstr(data_line, ", ,") || strstr(data_line, ",,"))
        {
            fprintf(stderr,"\nLine: %d; Too many commas\n", num_line);
            num_error++;
        }
        else if ((*data_line == ',') || (data_line[strlen(data_line) - 1] == ','))
        {
            fprintf(stderr,"\nLine: %d; Missing a number\n", num_line);
            num_error++;
        }

        p = strtok(data_line, ","); /* first call for strtok wont be NULL we checked the string before we called data() */
        while (p != NULL) /* a loop to add all the numbers to our table */
        {
            int i = 0,j;

            if(*p == ' ') p++;
            if(*p == '+') p++;
            if(p[(len = strlen(p))-1] == ' ') p[len - 1] = '\0';

            if(strchr(p, ' ')) /* i removed spaces on both sides of p so if we find space that means its in between 2 strings (or 2 numbers we still dont know) */
            {
                fprintf(stderr,"\nLine: %d; Missing a comma\n", num_line);
                num_error++;
                break;
            }
            
            if (*p == '-')
            {
                j = i;
                i++;
            }
            else j = i;

            for (; i < len; i++)
            {
                if (!isdigit(p[i]))
                {
                    fprintf(stderr,"\nLine: %d; The variable after # is not a number\n", num_line);
                    num_error++;
                    return 0;
                }
            }
    
            num = atoi(&p[j]);
            if ((num < -(MAX_INT_NUM + 1)) || (num > MAX_INT_NUM)) /* checking if it in range of signed 14 bit int */
            {
                fprintf(stderr,"\nLine: %d; One of your variables is too large\n", num_line);
                num_error++;
                break;
            }
            
            memory_table[MAX_MEMORY - 1 - *DC].number = num; /* storing the number and advancing the DC counter, yes even if we encontered a error, strtol will return 0 and later we just wont make the output files */
            (*DC)++;
            
            p = strtok(NULL, ","); /* next variable */
        }
    }
    else /* we got type = 's', s stands for string */
    {
        int i;
        len = strlen(data_line);

        if ((data_line[0] != '"') || (data_line[len-1] != '"'))
        {
            fprintf(stderr,"\nLine: %d; Missing quotation marks\n", num_line);
            num_error++;
        }
        else
        {
            data_line++;
            
            for (i = 0; i < len - 2; i++) /* adds each char ascii value */
            {
                memory_table[MAX_MEMORY - 1 - *DC].number = data_line[0];
                (*DC)++;
                data_line++;
            }  
            memory_table[MAX_MEMORY - 1 - *DC].number = '\0';
            (*DC)++;
        }         
    }
    
    return num_error;
}

int instruction(Word* memory_table,char name_comm[], char code_line[], int* IC, int num_line)  /* gets an instruction line cheking if its grammar correct and adds the word to the memory table */
{
    int num_error = 0, num_comm, main_line, num_reg1, num_reg2;
    char *par1, *par2;

    num_comm = isSavedWord(name_comm);
    if (num_comm == -1)
    {
        fprintf(stderr,"\nLine: %d; No command with the name %s exists\n", num_line, name_comm);
        num_error++;
        return num_error;
    }
    /* from 0-15, "mov","cmp","add","sub","not","clr","lea","inc","dec","jmp","bne","red","prn","jsr","rts","stop" */
    main_line = *IC; /* saving the line of the command word */
    if ((num_comm <= 3) || (num_comm == 6)) /* first group of commands */
    {
        par1 = strtok(code_line,",");
        par2 = strtok(NULL,",");
        if ((par1 == NULL) || (par2 == NULL) || (strtok(NULL,",") != NULL))
        {
            fprintf(stderr,"\nLine: %d; You need to provide two variables for the %s command\n", num_line, name_comm);
            num_error++;
            return num_error;
        }

        if (*par1 == ' ') par1++;
        if (*par2 == ' ') par2++;
              
        memory_table[main_line].bits.par1 = 0;
        memory_table[main_line].bits.par2 = 0;
        memory_table[main_line].bits.ERA = 0;
        memory_table[main_line].bits.opcode = num_comm;

        num_reg1 = isSavedWord(par1);
        num_reg2 = isSavedWord(par2);

        if (num_comm == 6 && ((MAX_NUM_COMM <= num_reg1) || (*par1 == '#'))) /* lea */
        {
            fprintf(stderr,"\nLine: %d; The source variable can not be a register or a number\n", num_line);
            num_error++;
            return num_error;
        }

        if ((MAX_NUM_COMM*2) <= (num_reg1 + num_reg2)) /* 32 is min number if both are registers */
        {
            memory_table[main_line].bits.source = 3;
            memory_table[main_line].bits.target = 3;
            (*IC)++;
            memory_table[*IC].bits.ERA = 0;
            memory_table[*IC].reg.source = num_reg1 - MAX_NUM_COMM;
            memory_table[*IC].reg.target = num_reg2 - MAX_NUM_COMM;
        }
        else
        {
            /* source */
            if (MAX_NUM_COMM <= num_reg1) /* source is register */
            {
                memory_table[main_line].bits.source = 3;
                (*IC)++;
                memory_table[*IC].reg.ERA = 0;
                memory_table[*IC].reg.source = num_reg1 - MAX_NUM_COMM;
                memory_table[*IC].reg.target = 0;
            }
            else if (*par1 == '#') /* source is a number */
            {
                if (isLegalNum(par1, &num_reg1, &num_error, num_line)) /* we reuse num_reg cause we wont need it anymore */
                {
                    memory_table[main_line].bits.source = 0;
                    (*IC)++;
                    memory_table[*IC].number = (num_reg1 << 2);
                }
                else return num_error;
            }
            else /* the only option stayed is source is a label */
            {
                memory_table[main_line].bits.source = 1;
                (*IC)++;
                memory_table[*IC].number = 0; /* we reset the line to 0s */
                memory_table[*IC].bits.ERA = 2; /* we still dont know if the label is extern or not, phaseTwo will hadle it */
            }

            /* target */
            if (MAX_NUM_COMM <= num_reg2) /* target is register */
            {
                memory_table[main_line].bits.target = 3;
                (*IC)++;
                memory_table[*IC].reg.ERA = 0;
                memory_table[*IC].reg.source = 0;
                memory_table[*IC].reg.target = num_reg2 - MAX_NUM_COMM;
            }
            else if (*par2 == '#') /* target is a number */
            {
                if (num_comm != 1) /* only cmp can have a number as target */
                {
                    fprintf(stderr,"\nLine: %d; The target variable can not be a number\n", num_line);
                    num_error++;
                    return num_error;
                }
                
                if (isLegalNum(par2, &num_reg2, &num_error, num_line)) /* we reuse num_reg cause we wont need it anymore */
                {
                    memory_table[main_line].bits.target = 0;
                    (*IC)++;
                    memory_table[*IC].number = (num_reg2 << 2);
                }
                else return num_error;
                
            }
            else /* the only option stayed is target is a label */
            {
                memory_table[main_line].bits.target = 1;
                (*IC)++;
                memory_table[*IC].number = 0; /* we reset the line to 0s */
                memory_table[*IC].bits.ERA = 2; /* we still dont know if the label is extern or not, phaseTwo will hadle it */
            }
        }
            
        
               
    }
    else if ((num_comm == 14) || (num_comm == 15)) /* third group of commands */
    {
        par1 = strtok(code_line,",");
        if (par1 != NULL)
        {
            fprintf(stderr,"\nLine: %d; You have too many variables for %s command\n", num_line, name_comm);
            num_error++;
            return num_error;
        }
        
        memory_table[main_line].bits.par1 = 0;
        memory_table[main_line].bits.par2 = 0;
        memory_table[main_line].bits.source = 0;
        memory_table[main_line].bits.target = 0;
        memory_table[main_line].bits.ERA = 0;
        memory_table[main_line].bits.opcode = num_comm;
    }
    else if (num_comm <= 15) /* the only group that remains is second group of commands */
    {
        par1 = strtok(code_line," ");
        if ((par1 == NULL) || (strtok(NULL," ") != NULL)) /* in this kind of commands we have only one variable and no spaces allowed in jump commands variable */
        {
            fprintf(stderr,"\nLine: %d; You need to provide only one variable for the %s command and space is not allowed\n", num_line, name_comm);
            num_error++;
            return num_error;
        }

        

        memory_table[main_line].bits.source = 0;
        memory_table[main_line].bits.ERA = 0;
        memory_table[main_line].bits.opcode = num_comm;

        if ((num_comm == 9) || (num_comm == 10) || (num_comm == 13)) /* jump commands */
        {
            memory_table[main_line].bits.target = 2;
            (*IC)++;
            memory_table[*IC].number = 0; /* we reset the line to 0s */
            memory_table[*IC].bits.ERA = 2; /* we still dont know if the label is extern or not, phaseTwo will hadle it */

            if (*par1 == '(') /* first char is '(' and we are missing the label */
            {
                fprintf(stderr,"\nLine: %d; Missing a label before parentheses\n", num_line);
                num_error++;
                return num_error;
            }   
            
            par2 = strchr(par1, ')');
            par1 = strchr(par1, '(');
            
            if ((par1 == NULL) && (par2 == NULL)) /* only the label that we need to jump to */
            {
                memory_table[main_line].bits.par1 = 0;
                memory_table[main_line].bits.par2 = 0;
            }
            else
            {
                if (par2 == NULL)
                {
                    fprintf(stderr,"\nLine: %d; Missing a closing parenthesis\n", num_line);
                    num_error++;
                    return num_error;
                }
                else if (par1 == NULL)
                {
                    fprintf(stderr,"\nLine: %d; Missing an openning parenthesis\n", num_line);
                    num_error++;
                    return num_error;
                }
                else /* we have both parentheses */
                {
                    if (par1 > par2) /* we have )...( */
                    {
                        fprintf(stderr,"\nLine: %d; Wrong placement of parentheses\n", num_line);
                        num_error++;
                        return num_error;
                    }
                    if (strchr(par1, ',') == NULL) /* missing comma */
                    {
                        fprintf(stderr,"\nLine: %d; You need to provide two variables inside the parentheses\n", num_line);
                        num_error++;
                        return num_error;
                    }

                    par1++; /* points after the '(' */
                    par1 = strtok(par1, ",");
                    *par2 = '\0'; /* replace the ')' */
                    par2 = strtok(NULL, ",");
                    if (par2 == NULL)
                    {
                        fprintf(stderr,"\nLine: %d; Wrong comma placement\n", num_line); /* if we get (......), */
                        num_error++;
                        return num_error;
                    }
                    if (strcmp(par1,"") == 0 || strcmp(par2,"") == 0) /* if we get (,....) or (....,) */
                    {
                        fprintf(stderr,"\nLine: %d; You need to provide two variables inside the parentheses\n", num_line);
                        num_error++;
                        return num_error;
                    } 

                    num_reg1 = isSavedWord(par1);
                    num_reg2 = isSavedWord(par2);

                    if ((MAX_NUM_COMM*2) <= (num_reg1 + num_reg2)) /* 32 is min number if both are registers */
                    {
                        memory_table[main_line].bits.par1 = 3;
                        memory_table[main_line].bits.par2 = 3;
                        (*IC)++;
                        memory_table[*IC].bits.ERA = 0;
                        memory_table[*IC].reg.source = num_reg1 - MAX_NUM_COMM;
                        memory_table[*IC].reg.target = num_reg2 - MAX_NUM_COMM;
                    }
                    else
                    {
                        /* par1 */
                        if (MAX_NUM_COMM <= num_reg1) /* par1 is register */
                        {
                            memory_table[main_line].bits.par1 = 3;
                            (*IC)++;
                            memory_table[*IC].reg.ERA = 0;
                            memory_table[*IC].reg.source = num_reg1 - MAX_NUM_COMM;
                            memory_table[*IC].reg.target = 0;
                        }
                        else if (*par1 == '#') /* par1 is a number */
                        {
                            if (isLegalNum(par1, &num_reg1, &num_error, num_line)) /* we reuse num_reg cause we wont need it anymore */
                            {
                                memory_table[main_line].bits.par1 = 0;
                                (*IC)++;
                                memory_table[*IC].number = (num_reg1 << 2);
                            }
                            else return num_error;
                            
                        }
                        else /* the only option stayed is par1 is a label */
                        {
                            memory_table[main_line].bits.par1 = 1;
                            (*IC)++;
                            memory_table[*IC].number = 0; /* we reset the word to 0s */
                            memory_table[*IC].bits.ERA = 2; /* we still dont know if the label is extern or not, phaseTwo will hadle it */
                        }

                        /* par2 */
                        if (MAX_NUM_COMM <= num_reg2) /* par2 is register */
                        {
                            memory_table[main_line].bits.par2 = 3;
                            (*IC)++;
                            memory_table[*IC].reg.ERA = 0;
                            memory_table[*IC].reg.source = 0;
                            memory_table[*IC].reg.target = num_reg2 - MAX_NUM_COMM;
                        }
                        else if (*par2 == '#') /* par2 is a number */
                        {
                            if (isLegalNum(par2, &num_reg2, &num_error, num_line)) /* we reuse num_reg cause we wont need it anymore */
                            {
                                memory_table[main_line].bits.par2 = 0;
                                (*IC)++;
                                memory_table[*IC].number = (num_reg2 << 2);
                            }
                            else return num_error;
                            
                        }
                        else /* the only option stayed is par2 is a label */
                        {
                            memory_table[main_line].bits.par2 = 1;
                            (*IC)++;
                            memory_table[*IC].number = 0; /* we reset the line to 0s */
                            memory_table[*IC].bits.ERA = 2; /* we still dont know if the label is extern or not, phaseTwo will hadle it */
                        }
                    }
                }    
            }      
        }
        else /* non jump commands */
        {
            memory_table[main_line].bits.par1 = 0;
            memory_table[main_line].bits.par2 = 0;
            num_reg1 = isSavedWord(par1);
            
            if ((num_comm == 12) && (*par1 == '#')) /* prn */
            {
                if (isLegalNum(par1, &num_reg1, &num_error, num_line))
                {
                    memory_table[main_line].bits.target = 0;
                    (*IC)++;
                    memory_table[*IC].number = (num_reg1 << 2); 
                }
                else  return num_error;              
            }
            else if (*par1 == '#') /* only prn can have a number as target */
            {
                fprintf(stderr,"\nLine: %d; The target variable can not be a number\n", num_line);
                num_error++;
                return num_error;
            }
            else /* we have a register or a label */
            {
                if (MAX_NUM_COMM <= num_reg1) /* target is register */
                {
                    memory_table[main_line].bits.target = 3;
                    (*IC)++;
                    memory_table[*IC].reg.ERA = 0;
                    memory_table[*IC].reg.source = 0;
                    memory_table[*IC].reg.target = num_reg1 - MAX_NUM_COMM;
                }
                else /* target is label */
                {
                    memory_table[main_line].bits.target = 1;
                    (*IC)++;
                    memory_table[*IC].number = 0; /* we reset the line to 0s */
                    memory_table[*IC].bits.ERA = 2; /* we still dont know if the label is extern or not, phaseTwo will hadle it */
                }
            }  
        }      
    }
    else /* isSavedWord returned 16+, that means its a register */
    {
        fprintf(stderr,"\nLine: %d; A register is not a type of command\n", num_line);
        num_error++;
        return num_error;
    }
    
    (*IC)++; /* in the function above we check the type off a variable and then adding a new word in memory to put it into, but after we added the variables we still point to last word so we need to point to next free word */
    return num_error;
}

void rearrangeTable(Word* memory_table, int IC, int DC)  /* swaps the order of elements in memory_table, all my data is at the end and i need to connect it to the end of IC */
{
    int i,j;
    Word temp;

    for ( i = IC, j = MAX_MEMORY - 1 ; i < j; i++, j--) /* IC is the next empty word for instructions */
    {
        temp = memory_table[i];
        memory_table[i] = memory_table[j];
        memory_table[j] = temp;
    }
    
}

void fixSymbols(Symbol* symbol_table, int symbol_table_size, int IC)  /* this function fixes the right address for data labels */
{
    int i;

    for ( i = 0; i < symbol_table_size - 1; i++) /* symbol_table_size started from 1 and not 0 */
    {
        if (symbol_table[i].type == 'I') continue; /* I stands for instruction, we only have 2 types, so the other option is D that stands for data */

        symbol_table[i].address += IC; /* add the IC counter, for example: first label for data has adress of 0 but it should be the first one where IC now points */
    }
    
}

int fixEntry(Symbol* symbol_table, int symbol_table_size, Symbol* entry_table, int entry_table_size) /* updates the adress for entry labes */
{
    int i, index;

    for ( i = 0; i < entry_table_size; i++)
    {
        index = findSymbol(symbol_table, symbol_table_size, entry_table[i].name);

        if (index == -1) return 0;

        entry_table[i].address = symbol_table[index].address;
    }
    
    return 1;
}
