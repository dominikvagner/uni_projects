/**
 *  @file generator.h
 *
 *  @brief Header file for generator.c (IFJCode21 generator).
 *
 *  @author <xvagne10> Dominik Vágner
 */

#ifndef generator_H
#define generator_H

#include "str.h"
#include "tokenList.h"
#include "error.h"

/**
 * @brief Help function to generate IFJcode21 for all tokens in list. 
 * 
 * @param list List cointaing tokens for which is code generated.
 */
void generate_code_from_list(DLList *list);

/**
 * @brief Function generates IFJcode21 from single token.
 * 
 * @param token Token for which is code generated.
 */
void generate_code_from_token (token_ptr *token, string *main);

/**
 * @brief Function generates IFJcode21 for builtin functions of IFJ21.
 * 
 * @param code Strign which the generated code is appended to.
 */
void generate_built_in_code(string *code); 

#endif