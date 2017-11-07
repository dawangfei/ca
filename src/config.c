#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define PFL_MEMO_CHARACTER       '#'
#define PFL_LINE_BUFFER_SIZE     1024    /* before PFL_MEMO_CHARACTER or '\n' */
#define PFL_FILE_NAME_LEN        1024
#define SUCCESS                  0
#define FAILURE                  -1
#define FALSE	                 0
#define TRUE	                 1

static void dc_cfg_clear_blank(char *);
static int dc_cfg_is_section(char *);
static int dc_cfg_is_this_section(const char *, const char *);
static int dc_cfg_is_this_entry(const char *, const char *);
static void dc_cfg_cut_content(char *, char *, int);


/*
 *  Function:  dc_cfg_clear_blank
 *
 *      To remove all comment and blank char from one line
 *
 *  Parameters:
 *
 *      line - point of a string of a line
 *
 *  Return Value:
 */

static void dc_cfg_clear_blank(char *line)
{
    int i = 0, j, k;
    char buf[PFL_LINE_BUFFER_SIZE];

    while (line[i] != 0) 
    {
        if (line[i] == PFL_MEMO_CHARACTER || line[i] == '\n') 
        {
            line[i] = 0;
            if (i == 0) break;
            j = i - 1;
            do 
            {
                if (j < 0) break;
                if ((line[j] != ' ') && (line[j] != '\t')) break;
                line[j--] = 0;
            } while(1);
            break;
        }    
        i++;
    }
 
    i = 0;
    memset(buf, 0, PFL_LINE_BUFFER_SIZE);

    while ((line[i] != '=') && (i < (int)strlen(line)))
        i++;

    if (i == (int)strlen(line)) return;

    for (j = i - 1; (line[j] == ' ') || (line[j] == '\t'); j--);
    for (k = i + 1; (line[k] == ' ') || (line[k] == '\t'); k++);

    memcpy(buf, line, j + 1);
    buf[j + 1] = '=';
    strcat(buf + j + 2, line + k);
 
    strcpy(line, buf);
}


/*
 *  Function:  dc_cfg_is_section
 *
 *      To confirm if a line is a section line
 *
 *  Parameters:
 *
 *      line - point of a string of a line
 *
 *  Return Value:
 *
 *       0 - is not a section line
 *       1 - is a section line
 */

static int dc_cfg_is_section(char * line)
{
    return line[0] == '[';
}


/*
 *  Function:  dc_cfg_is_this_section
 *
 *      To confirm if this line is a spec section
 *
 *  Parameters:
 *
 *      line    - point of a string of a line
 *      section - name of section
 *
 *  Return Value:
 *
 *       0 - is not this section
 *       1 - is this section
 */

static int dc_cfg_is_this_section(const char *line, const char *section)
{
    return !memcmp(line + 1, section, strlen(section));
}


/*
 *  Function:  dc_cfg_is_this_entry
 *
 *      To confire if this line is a spec entry
 *
 *  Parameters:
 *
 *      line  - point of a string of a line
 *      entry - name of entry
 *
 *  Return Value:
 *
 *       0 - is not this entry
 *       1 - is this entry
 */

static int dc_cfg_is_this_entry(const char *line, const char *entry)
{
    return (!memcmp(line, entry, strlen(entry)) && line[strlen(entry)] == '=') ;
} 


/*
 *  Function:  dc_cfg_cut_content
 *
 *      To get the value from a line
 *
 *  Parameters:
 *
 *      line  - point of a string of a line
 *      dest  - value of entry to put
 *
 *  Return Value:
 */

static void dc_cfg_cut_content(char *line, char *dest, int size)
{
    int i = 0;

    while (line[i++] != '=');

    snprintf(dest, size, "%s", line + i);
} 


/*
 *  Function:  dc_cfg_get_value
 *
 *      To get a string value from profile
 *
 *  Parameters:
 *
 *      section  - section name
 *      entry    - entry name
 *      filename - full filename of profile
 *      value    - pointer of string which value is returned
 *
 *  Return Value:
 *
 *      SUCCESS - success
 *      FAILURE - failure
 */
int dc_cfg_get_value(const char *section,
                           const char *entry,
                           const char *filename,
                           char       *value,
                           int         size)
{
    FILE  *fp = NULL;
    char  line[PFL_LINE_BUFFER_SIZE] = {0};
    int cbNum = FALSE;
    int InThisSection = FALSE;
    
    if ((fp = fopen(filename, "r")) == NULL)
        return FAILURE;

    while (NULL != fgets(line, PFL_LINE_BUFFER_SIZE, fp)) 
    {
        dc_cfg_clear_blank(line);

        if (dc_cfg_is_section(line)) 
        {
            InThisSection = dc_cfg_is_this_section(line, section);
            continue;
        }

        if (InThisSection == FALSE) continue;

        if (dc_cfg_is_this_entry(line, entry)) 
        {
            dc_cfg_cut_content(line, value, size);
            cbNum = TRUE;
            break;
        }

        memset(line, 0, sizeof(line));
    }

    fclose(fp);

    if (cbNum == FALSE)
        return FAILURE;

    return SUCCESS;
}

/* config.c */
