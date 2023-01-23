/**
 *  Project: Implementation of imperative language compiler IFJ21
 *  @file error.h
 *
 *  @brief Header file with define exit error codes.
 *
 *  @author <xvagne10> Dominik Vágner
 */

#ifndef error_H
#define error_H

#define ERR_LEX             1
#define ERR_SYNTAX          2
#define ERR_SMNTIC_UNDEF    3
#define ERR_SMNTIC_TYPE     4
#define ERR_SMNTIC_PARAMS   5
#define ERR_SMNTIC_EXPR     6
#define ERR_SMNTIC_OTHER    7
#define ERR_RUN_NILL        8
#define ERR_RUN_ZERODIV     9
#define ERR_INTERNAL        99

#define ERR_SMNTIC_REDEFINE_F   12
#define ERR_SMNTIC_REDEFINE_V   13
#define ERR_SMNTIC_UNDEFINED_F  14
#define ERR_SMNTIC_UNDEFINED_V  15
#define ERR_SMNTIC_NUMBER_OF_PARAMS 16
#define ERR_SMNTIC_NUMBER_OF_RETURN_PARAMS 17
#define ERR_SMNTIC_PARAMS_TYPE 18
#define ERR_SMNTIC_RETURN_PARAMS_TYPE 19

#include "tokenList.h"

/**
 * @brief Implementation of printing error codes <br>
 * @param error code number
 */
void err_call(int error, token_ptr token);

#endif