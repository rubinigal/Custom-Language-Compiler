#include "header.h"
#include "util.h"

void removeExtraWhitespace(char *str) /* gets a string and removes extra whitespace */
{
    int i = 0, j = 0, bool_start_str = 1, bool_space_counter = 0;

    if(str[i] == '\0') return; /* fgets got only \n as the input string and changed it to \0 */

    while (j < strlen(str))
    {
        if (bool_start_str) /* clears all the whitespace from the start of the string */
        {
            if(isspace(str[j]))
            {
                j++;
                continue;
            }
            bool_start_str = 0;
        }
        if(isspace(str[j]))
        {
            bool_space_counter++;
            if(bool_space_counter == 1) /* we only copy one space between words */
            {
                str[i] = ' ';
                i++;
            }
            j++;
            continue;
        }
        str[i] = str[j];
        i++;
        j++;
        bool_space_counter = 0;
    }
    
    if ((i > 0) && (str[i-1] == ' ')) str[i-1] = '\0'; /* string might end on a space */
    str[i] = '\0';  /* terminate the output string */
}

int isSavedWord(char str[]) /* checks if the string we get is a saved word and return its index, else return -1 */
{
    int i,len;
    len = strlen(str);

    if (*str == ' ') str++;
    if (str[len - 1] == ' ') str[len - 1] = '\0';

    for (i = 0; i < MAX_SAVED_WORDS; i++)
    {
        if(strcmp(saved_words[i], str) == 0) return i;
    }
    
    return -1;
}

int isNumOrAlpha(char str[]) /* gets a string and checks that it contains only letters and numbers */
{
    int i;

    for (i = 0; i < strlen(str); i++) 
    {
        if (!isalnum(str[i])) 
        {
            return 0;  /* the string contains a character that is not a letter or digit */
        }
    }

    return 1;  /* the string contains only letters and digits */
} 

int isLegalNum(char str[], int* num ,int* num_error, int num_line) /* check if string a number, returns false/true (0/1) and saves the number in num */
{
    int i = 0,j, len;

    if (*str == '#') str++;
    if (*str == '+') str++;

    len = strlen(str);  
    if (str[len - 1] == ' ') str[len - 1] = '\0'; /* removing last space */

    if (strchr(str, ' ') != NULL) /* no spaces between # and end of number */
    {
        fprintf(stderr,"\nLine: %d; No spaces allowed from # to end of number\n", num_line);
        (*num_error)++;
        return 0;
    }

    if (*str == '-')
    {
        j = i;
        i++;
    }
    else j = i;

    for (; i < len; i++)
    {
        if (!isdigit(str[i]))
        {
            fprintf(stderr,"\nLine: %d; The variable after # is not a number\n", num_line);
            (*num_error)++;
            return 0;
        }
    }
    
    *num = atoi(&str[j]);
    if ((*num < -(MAX_INT_NUM/4 + 1)) || (*num > MAX_INT_NUM/4)) /* checking if it in range of signed 12 bit int */
    {
        fprintf(stderr,"\nLine: %d; One of your variables is too large\n", num_line);
        (*num_error)++;
        return 0;               
    }

    return 1;
}

void printBinary(int num) /* prints number as binary including fowarrding zeros */
{
    int i,len = 14; /* our word is only 14 bits */

    for (i = 0; i < len; i++) 
    {
        if (num & (1 << (len - 1 - i))) putchar('1');
        else putchar('0');
    }
}

void btou(int num , FILE* file) /* prints the number in a special binaly base */
{
    int i,len = 14; /* our word is only 14 bits */

    for (i = 0; i < len; i++) 
    {
        if (num & (1 << (len - 1 - i))) putc('/', file);
        else putc('.', file);
    }
}