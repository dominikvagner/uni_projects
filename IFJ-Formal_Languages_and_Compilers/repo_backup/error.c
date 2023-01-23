/**
 *  Project: Implementation of imperative language compiler IFJ21
 *  @file error.c
 *
 *  @brief Implementation of printing error codes
 *
 *  @author <xpoliv06> Tomáš Polívka
 *  @author <xhajek51> Vojtěch Hájek
 */
#include "error.h"
#include "tokenList.h"
#include <stdio.h>
#include <stdlib.h>


void err_call(int error, token_ptr token) {
    switch (error) {
        case ERR_LEX:
          fprintf(stderr, "ERR 1 - Lexical error\n");
          break;
        case ERR_SYNTAX:
            fprintf(stderr, "ERR 2 - Syntax error");
            if (token != NULL)
            {
                fprintf(stderr, " on line %d, near the colum %d\n", token->line_num, token->col_num);
            }
            break;
        case ERR_SMNTIC_UNDEF:
            fprintf(stderr, "ERR 3 - Semantic error (indefinite function or variable..)\n");
            break;
        case ERR_SMNTIC_TYPE:
            fprintf(stderr, "ERR 4 - Semantic error (type incompatibility)\n");
            fprintf(stderr, "Trying to assign another data type on line %d, near the colum %d\n", token->line_num, token->col_num);
            break;
        case ERR_SMNTIC_PARAMS:
            fprintf(stderr, "ERR 5 - Semantic error (wrong number / type of parameters or return values when calling a function or returning from a function");
            break;
        case ERR_SMNTIC_EXPR:
            fprintf(stderr, "ERR 6 - Semantic error (type incompatibility in arithmetic, string or relational expressions) \n");
            fprintf(stderr, "Trying to assign another data type on line %d, near the colum %d\n", token->line_num, token->col_num);
            break;
        case ERR_SMNTIC_OTHER:
            fprintf(stderr, "ERR 7 - Semantic error (other)");
            break;
        case ERR_RUN_NILL:
            fprintf(stderr, "ERR 8 - Runtime error when working with unexpected nil value\n");
            fprintf(stderr, "Trying to assign nill on line %d, near the colum %d\n", token->line_num, token->col_num);
            break;
        case ERR_RUN_ZERODIV :
            fprintf(stderr, "ERR 9 - Runtime error of integer division by zero constant ");
            break;
        case ERR_INTERNAL:
            fprintf(stderr, "ERR 99 - Internal compiler error");
            break;
        case ERR_SMNTIC_REDEFINE_F:
        case ERR_SMNTIC_REDEFINE_V:
            fprintf(stderr, "ERR 3 - Semantic error (indefinite function or variable..)\n");
            if (token != NULL)
            {
                fprintf(stderr, "Trying to redefine %s %s on line %d, near the colum %d\n", (error == ERR_SMNTIC_REDEFINE_F ? "function" : "value"), token->data->string->data, token->line_num, token->col_num);
            }
            error = ERR_SMNTIC_UNDEF;
            break;

        case ERR_SMNTIC_UNDEFINED_F:
        case ERR_SMNTIC_UNDEFINED_V:
            fprintf(stderr, "ERR 3 - Semantic error (indefinite function or variable..)\n");
            if (token != NULL)
            {
                if (error == ERR_SMNTIC_UNDEFINED_F){
                    fprintf(stderr, "Trying to call undefined function %s on line %d, near the colum %d\n", token->data->string->data, token->line_num, token->col_num);
                }
                else{
                    fprintf(stderr, "Trying to access undefined value %s on line %d, near the colum %d\n", token->data->string->data, token->line_num, token->col_num);
                }
            }
            error = ERR_SMNTIC_UNDEF;
            break;

        case ERR_SMNTIC_NUMBER_OF_PARAMS:
        case ERR_SMNTIC_NUMBER_OF_RETURN_PARAMS:
            fprintf(stderr, "ERR 5 - Semantic error (wrong number / type of parameters or return values when calling a function or returning from a function\n");
            if (token != NULL)
            {
                fprintf(stderr, "Wrong number of %sparams in function %s on line %d, near the colum %d\n", (error == ERR_SMNTIC_NUMBER_OF_PARAMS ? "" : "return "), token->data->string->data, token->line_num, token->col_num);
            }
            error = ERR_SMNTIC_PARAMS;
            break;

        case ERR_SMNTIC_PARAMS_TYPE:
        case ERR_SMNTIC_RETURN_PARAMS_TYPE:
            fprintf(stderr, "ERR 5 - Semantic error (wrong number / type of parameters or return values when calling a function or returning from a function\n");
            if (token != NULL)
            {
                fprintf(stderr, "Wrong types of %sparams in function %s on line %d, near the colum %d\n", (error == ERR_SMNTIC_PARAMS_TYPE ? "" : "return "), token->data->string->data, token->line_num, token->col_num);
            }
            error = ERR_SMNTIC_PARAMS;
            break;

    }
    exit(error);
}

