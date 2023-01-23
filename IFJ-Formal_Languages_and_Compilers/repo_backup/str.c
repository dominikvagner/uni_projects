/**
 *  Project: Implementation of imperative language compiler IFJ21
 *  @file str.c
 *
 *  @brief Library that enables working with dynamic strings
 *
 *  @author <xvasin11> Ladislav Vašina
 */

#include "str.h"
#include <malloc.h>
#include <stdio.h>
#include <string.h>

#define STR_OK 1
#define STR_ERR 0

#define STR_LEN_INC 8


int strInit(string *s){
    s->data = (char*) malloc(STR_LEN_INC); // String allocation 
    if(s->data != NULL){ // String allocation OK
        s->data[0] = '\0';
        s->length = 0;
        s->capacity = STR_LEN_INC;
        return STR_OK;
    }else{ // String allocation FAILED
        return STR_ERR;
    }
}


void strFree(string *s){
    free(s->data);
}


void strClear(string *s) {
  s->data[0] = '\0';
  s->length = 0;
}


int strAppendChar(string *s1, char toAppend){
    if((s1->length + 1) < s1->capacity){ // If there is enough of the allocated space
        s1->data[s1->length] = toAppend;
        s1->length++;
        s1->data[s1->length] = '\0';
        return STR_OK;
    }else{ // There is not enough of the allocated memory --> increase
        s1->data = (char *) realloc(s1->data, sizeof(char) * (s1->length + STR_LEN_INC));
        if(s1->data == NULL){
            return STR_ERR; // Allocation failed
        }
        s1->capacity = s1->capacity + STR_LEN_INC; 
        s1->data[s1->length] = toAppend;
        s1->length++;
        s1->data[s1->length] = '\0';
        return STR_OK;
    }
}

int strAppendStr(string *s1, const char *toAppendStr){
    int const_string_length = (int) strlen(toAppendStr);

    for (int i = 0; i < const_string_length; i++) {
      if (strAppendChar(s1, toAppendStr[i]) == STR_ERR) {
        return STR_ERR;
      }
    }
    return STR_OK;
}


int strCopyString(string *s1, string *s2){
    if(s2->length >= s1->capacity){ // If there is NOT enough memory allocated
        s1->data = (char *) realloc(s1->data, s2->length + 1); // Allocate enough space for s2
            if(s1->data == NULL){
                return STR_ERR; // Allocation failed
            }
        s1->capacity = s2->length + 1; // Change capacity parameter of 's1'(destination string where we copy second string)
    }
    strcpy(s1->data, s2->data); // Copy 's2' to 's1'
    s1->length = s2->length;
    return STR_OK;
}


int strCopyConstant(string *s1, char *c){
    int newLength = strlen(c);
    if(newLength >= s1->capacity){ // If there is NOT enough memory allocated
        s1->data = (char *) realloc(s1->data, newLength + 1); // Allocate enough space for 's2'
            if(s1->data == NULL){
                return STR_ERR; // Allocation failed
            }
        s1->capacity = newLength + 1; // Change capacity parameter of 's1'(destination string where we copy second string)
    }
    strcpy(s1->data, c); // Copy 'c' to 's1'
    s1->length = newLength;
    return STR_OK;
}

void asciiConvert(char *s) {
  printf("string@");
  for (int i = 0; i <= (int)strlen(s) - 1; i++) {
    int currentAsciiVal = (int)s[i];
    if (currentAsciiVal <= 32 && currentAsciiVal > 0) {
      printf("\\%03d", currentAsciiVal);
    } else if (currentAsciiVal == 35) {
      printf("\\035");
    } else if (currentAsciiVal == 39) {
      continue;
    } else if (currentAsciiVal == 92) {
      printf("\\092");
    } else {
      printf("%c", s[i]);
    }
  }
}

int strCompareString(string *s1, string *s2){
    return strcmp(s1->data, s2->data);
}


int strCompareConstant(string *s1, char *s2){
    return strcmp(s1->data, s2);
}


char *strGetString(string *s){
    return s->data;
}


int strGetLength(string *s){
    return (int) strlen(s->data);
}


int strGetCapacity(string *s){
    return s->capacity;
}