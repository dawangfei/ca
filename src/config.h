#ifndef __DC_CONFIG_H_
#define __DC_CONFIG_H_


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
                           int         size);



#endif
