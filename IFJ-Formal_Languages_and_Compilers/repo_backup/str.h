/**
 *  Project: Implementation of imperative language compiler IFJ21
 *  @file str.h
 *
 *  @brief Header file for str.c (Header file that enables working with dynamic strings)
 *
 *  @author <xvasin11> Ladislav Vašina
 */


#ifndef STRING_H
#define STRING_H


/**
 * @brief string structure
 */
typedef struct{
    char* data;    // Var to store the string
    int length;   // String length
    int capacity; // Memory allocated
} string;

/**
 * @brief Function that initializes string.
 *
 * @param s Pointer to string that we want to initialize.
 * @return STR_OK (1) string if initialization is successful. STR_ERR (0) if string if initialization is NOT successful.
 */
int strInit(string *s);

/**
 * @brief Function that clears string from the memory.
 *
 * @param s Pointer to string that we want to free from the memory.
 */
void strFree(string *s);

/**
 * @brief Function deletes (clears) string data
 */
void strClear(string *s);

/**
 * @brief Function that appends char at the end of the string 's1'.
 *
 * @param s1 String that we want to append string to.
 * @param toAppend Char that we want to append at the end of 's1'.
 * @return STR_OK (1) is returned if append was successful. STR_ERR (0) is returned if append was NOT successful.
 */
int strAppendChar(string *s1, char toAppend);

/**
 * @brief Function that appends string at the end of the string 's1'.
 *
 * @param s1 String that we want to append string to.
 * @param toAppendStr String that we want to append at the end of 's1'.
 *
 * @return STR_OK (1) is returned if append was successful. STR_ERR (0) is returned if append was NOT successful.
 */
int strAppendStr(string *s1, const char toAppendStr[]);

/**
 * @brief Function that copies string 's2' to the string 's1'.
 *
 * @param s1 String that we want to copy to.
 * @param s2 String that we will copy to 's1'.
 * @return STR_OK (1) is returned if copying was successful. STR_ERR (0) is returned if copying was NOT successful.
 */
int strCopyString(string *s1, string *s2);

/**
 * @brief Function that copies constant 'c' to 's1'.
 *
 * @param s1 String that we want to copy to.
 * @param c  Constant that we want to copy to 's1'.
 * @return STR_OK (1) is returned if copying was successful. STR_ERR (0) is returned if copying was NOT successful.
 */
int strCopyConstant(string *s1, char *c);

/**
 * @brief Function converts unprintable chars as ascii values.
 *
 * @param s String that we want to check for unprintable chars.
 */
void asciiConvert(char *s);

/**
 * @brief Function that compares string 's1' and 's2'.
 *
 * @param s1 First string that we want to compare.
 * @param s2 Second string that we want to compare.
 * @return STR_OK (1) is returned if compare was successful. STR_ERR (0) is returned if compare was NOT successful.
 */
int strCompareString(string *s1, string *s2);

/**
 * @brief Function that compares string 's1' and constant 's2'.
 *
 * @param s1 First string that we want to compare.
 * @param s2 Second string constant that we want to compare.
 * @return STR_OK (1) is returned if compare was successful. STR_ERR (0) is returned if compare was NOT successful.
 */
int strCompareConstant(string *s1, char *s2);

/**
 * @brief Function that returns value of string 's'.
 *
 * @param s String that we want to get value of.
 * @return Value of string.
 */
char *strGetString(string *s);

/**
 * @brief Function that returns length of string 's'.
 *
 * @param s String that we want to get length of.
 * @return Length of the given string.
 */
int strGetLength(string *s);

/**
 * @brief Function that returns data size (capacity) of string 's'.
 *
 * @param s String that we want to get capacity of.
 * @return Capacity of the given string.
 */
int strGetCapacity(string *s);


#endif