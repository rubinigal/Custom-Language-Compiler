#include "header.h"
#include "fileMaker.h"

void fileMaker(Word* memory_table, int IC, int DC, Symbol* entry_table, int entry_table_size, Symbol* extern_table, int extern_table_size, const char file_name[])
{
    FILE *ob, *ent, *ext; /* output files */
    int error = 0, table_index;
    char *file_name_extension = (char *)malloc((strlen(file_name) + 5) * sizeof(char));   /* we make space for the name of the file + its extension */

    if (file_name_extension == NULL) /*checking if malloc worked*/
    {
        fprintf(stderr,"\nMemory allocation failed\n");
        exit(2);
    }

    if (IC + DC > 0) /* making .ob file */
    {
        strcpy(file_name_extension, file_name);
        strcat(file_name_extension, ".ob");
        if ((ob = fopen(file_name_extension, "w")) == NULL)
        {
            fprintf(stderr, "\nsource code file %s could not be opened\n", file_name_extension);
            error++;
        }
    }
    if (entry_table_size > 0) /* making .ent file */
    {
        strcpy(file_name_extension, file_name);
        strcat(file_name_extension, ".ent");
        if ((ent = fopen(file_name_extension, "w")) == NULL)
        {
            fprintf(stderr, "\nsource code file %s could not be opened\n", file_name_extension);
            error++;
        }      
    }
    if (extern_table_size > 0) /* making .ext file */
    {
        strcpy(file_name_extension, file_name);
        strcat(file_name_extension, ".ext");
        if ((ext = fopen(file_name_extension, "w")) == NULL)
        {
            fprintf(stderr, "\nsource code file %s could not be opened\n", file_name_extension);
            error++;
        }
    }
    if (error > 0) /* if one of the files failed to open we need to delete the files that opened and return */
    {
       if (ob != NULL) 
        {
            fclose(ob);
            strcpy(file_name_extension, file_name);
            strcat(file_name_extension, ".ob");
            remove(file_name_extension);
        }
        if (ent != NULL) 
        {
            fclose(ent);
            strcpy(file_name_extension, file_name);
            strcat(file_name_extension, ".ent");
            remove(file_name_extension);
        }
        if (ext != NULL) 
        {
            fclose(ext);
            strcpy(file_name_extension, file_name);
            strcat(file_name_extension, ".ext");
            remove(file_name_extension);
        }
        
        free(file_name_extension);
        return;
    }

    if (entry_table_size > 0) /* writing to .ent file */
    {
        for (table_index = 0; table_index < entry_table_size ; table_index++) /* filling up the file */
        {
            fputs(entry_table[table_index].name, ent);
            fputc('\t', ent);
            fprintf(ent,"%d\n",entry_table[table_index].address);
        }
    }
    if (extern_table_size > 0) /* writing to .ext file */
    {
        for (table_index = 0; table_index < extern_table_size ; table_index++) /* filling up the file */
        {
            fputs(extern_table[table_index].name, ext);
            fputc('\t', ext);
            fprintf(ext,"%d\n",extern_table[table_index].address);
        }
    }

    if (IC + DC > 0) /* writing to .ob file */
    {
        fprintf(ob,"\t%d %d", IC, DC);
        for ( table_index = 0; table_index < (IC + DC) ; table_index++)
        {
            fputc('\n', ob);
            if (table_index + MEMORY_START >= 1000)
            {
                fprintf(ob,"%d\t", table_index + MEMORY_START);
            }
            else
            {
                fprintf(ob,"%c%d\t", '0', table_index + MEMORY_START);
            }

            btou(memory_table[table_index].number, ob);       
        }
    }
    
    printf("\nThe output files for the file \"%s\" has been created\n", file_name);

    free(file_name_extension);
    if (ob != NULL) 
    {
    fclose(ob);
    }
    if (ent != NULL) 
    {
    fclose(ent);
    }
    if (ext != NULL) 
    {
    fclose(ext);
    }
}