/**
 *  Project: Implementation of imperative language compiler IFJ21
 *  @file scanner.h
 *
 *  @brief Header file for scanner.c (Lexical analyzer).
 *
 *  @author <xvagne10> Dominik Vágner
 */

#ifndef scanner_H
#define scanner_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "tokenList.h"

#define str(x) #x
#define xstr(x) str(x)

/**
 * @brief Enum of all IFJ21 keywords.
 */
typedef enum {
    DO,
    ELSE,
    END,
    FUNCTION,
    GLOBAL,
    IF,
    INTEGER,
    LOCAL,
    NIL,
    NUMBER,
    REQUIRE,
    RETURN,
    STRING,
    THEN,
    WHILE
} keywords;

/**
 * @brief Enum of all states for lexical analysis FSM. 
 */
typedef enum {
	S_START,
    S_SPACE,
    S_SUB,
    S_COMMENT0,
    S_COMMENT1,
    S_COMMENT2,
    S_COMMENT3,
    S_ADD,
    S_MUL,
    S_DIV,
	S_IDIV,
	S_STRLEN,
    S_LEFT_PAR,
	S_RIGHT_PAR,
	S_DOUBLE_DOT,
    S_ASSIGN,
    S_EQL,          // ==
	S_GT,           // >
	S_GTE,          // >=
	S_LT,           // <
	S_LTE,          // <=
    S_COMMA,
	S_ID,
    S_INT,
	S_INT0,
    S_HEX0,
	S_HEX1,
	S_FP_DOT,
    S_NUMBER,
    S_EXP0,
	S_EXP1,
    S_EXP2,
	S_EOL,
	S_EOF,
	S_DOT0,
	S_DOT1,
    S_TILDA,
	S_NEQL,
	S_STR_START,    // "
    S_STR_T1,       // \?                T8
    S_STR_T2,       // \0                T9
    S_STR_T3,       // \00               T10
    S_STR_T4,       // \0(1-9)  \1(0-9)  T11
    S_STR_T5,       // \1                T12
    S_STR_T6,       // \2                T13
    S_STR_T7,       // \2(0-4)           T14
    S_STR_T8,       // \25               T15
    S_STR_FIN,       // "
    S_NULL,
    S_ERR
} lexfsm_state;

/**
 * @brief Function to read whole input and append all tokens to a token list. 
 * 
 * @param first Pointer to first token in list. Insert new tokens after.
 * @return int State code, If there aren't any errors ? 0 : 2 
 */
int get_token_list(DLList *list);

/**
 * @brief Read single token from stdin and insert into passed token pointer.
 * 
 * @param insert_into Token pointer into which are data supposed to be inserted.
 * @return int State code, If there aren't any errors ? 0 : 2
 */
int get_single_token(token_ptr *insert_into);

/**
 * @brief Determine if token is keyword from IFJ21
 * 
 * @param token Token with ID type who can be a token.
 * @return true If token is a keyword.
 * @return false If Token isn't a keyword.
 */
bool is_token_keyword(token_ptr *token);

/**
 * @brief Print all tokens from token list to stdout. 
 * 
 * @param first 
 */
void print_token_list(DLList *list);

/**
 * @brief Print a single token to stdout. 
 * 
 * @param token Token to be printed.
 */
void print_single_token(token_ptr token);

/**
 * @brief Finite state machine which decides the type of token.
 * 
 * @param token Token pointer whomst type we need to determine.
 * @return int State code, If there aren't any errors ? 0 : 2 
 */
int lex_fsm(token_ptr *token);

#endif