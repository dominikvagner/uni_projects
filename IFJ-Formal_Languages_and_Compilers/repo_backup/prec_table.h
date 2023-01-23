/**
 *  Project: Implementation of imperative language compiler IFJ21
 *  @file prec_table.h
 *
 *  @brief Header file for prec_table.c (File that implements precedence table)
 *
 *  @author <xvasin11> Ladislav Vašina
 *  @author <xhajek51> Vojtěch Hájek
 */
#ifndef IFJ_PREC_TABLE_H
#define IFJ_PREC_TABLE_H

#include "tokenList.h"
#include "stack.h"
#include "stdbool.h"

/**
 * @enum Precedence table symbols
 * 
 */
typedef enum{
    S,  // < shift
    E,  // = equal
    R,  // > reduce
    X   // X error
} prec_table_actions;

/**
 * @enum Precedence table symbols
 * 
 */
typedef enum{
    PLUS,           // +
    MINUS,          // -
    MUL,            // *
    DIV,            // /
    IDIV,           // //
    LT,             // <
    GT,             // >
    LEQ,            // <=
    MEQ,            // >=
    NEQ,            // ~=
    EQ,             // ==
    LEFT_BRACKET,   // (
    RIGHT_BRACKET,  // )
    ID,             // i
    LENGHT,         // #
    CONCAT,         // ..
    END_DOLLAR      // $
} prec_table_symbols;

typedef enum{
    R_ADD,         // E -> E + E
    R_SUB,        // E-> E - E
    R_MUL,          // E -> E * E
    R_DIV,          // E -> E / E
    R_IDIV,         // E -> E // E
    R_LT,         // E -> E < E
    R_GT,         // E -> E > E
    R_LTE,          // E -> E <= E
    R_GTE,          // E -> E >= E
    R_NEQL,       // E -> E ~= E
    R_EQL,          // E -> E == E
    R_PARS,      // E -> (E)
    R_ID,           // E -> i
    R_STRLEN,       // E -> #E
    R_CONCAT,        // E -> E .. E
    R_NOTDEFINED    // NONDEFINED RULE
} prec_parsing_rules;

/**
 * @brief Function that processes expressions
 *
 * @param list List of tokens.
 * @param where_expression Flag of expression placement.
 * @param stackBst Stack that we use to store variables.
 * @param exp_type Type of given the expression.
 * @return Returns one token of T_K_EXPRESSION type.
 */
token_ptr expression(DLList *list, int where_expression, Stack_Bst *stackBst, token_type exp_type);

/**
 * @brief Function that returns index of given token in prec_table.
 *
 * @param token_table Token that we want to find in prec_table.
 * @param a Flag of expression placement.
 * @return
 */
int number_in_table(token_ptr token_table, int a);

/**
 * @brief Function that returns which rule will aplly.
 *
 * @param symbCount number of symbols in rule.
 * @param tokens Pointer to table of tokens.
 * @return
 */
prec_parsing_rules check_rule(int symbCount, token_ptr *tokens);

/**
 * @brief Function that returns string of one part postfix.
 *
 * @param string_token Pointer to token.
 * @return
 */
char *string_postfix(token_ptr string_token);


/**
 * @brief Function that returns operation for semantic control of expression.
 *
 * @param token_operation Pointer to token.
 * @return
 */
int operation(token_ptr token_operation);

#endif