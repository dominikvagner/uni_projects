/**
 *  Project: Implementation of imperative language compiler IFJ21
 *  @file stack.c
 *
 *  @brief Implementation of stack.
 *
 *  @author <xhajek51> Vojtěch Hájek
 */

#include "stack.h"
#include "tokenList.h"
#include "error.h"

int STACK_SIZE = MAX_STACK;

#define STACKDEC(T, TNAME)                                              \
    void Stack_##TNAME##_Init(Stack_##TNAME *stack ) {                  \
        if(stack == NULL){                                              \
            err_call(ERR_INTERNAL, NULL);                               \
        }                                                               \
        else{                                                           \
            stack->topIndex = -1;                                       \
        }                                                               \
    }                                                                   \
    int Stack_##TNAME##_IsEmpty(Stack_##TNAME *stack ) {                \
        return (stack->topIndex == -1);                                 \
    }                                                                   \
    int Stack_##TNAME##_IsFull(Stack_##TNAME *stack ) {                 \
        return (stack->topIndex == STACK_SIZE-1);                       \
    }                                                                   \
    void Stack_##TNAME##_Top(Stack_##TNAME *stack, T *data ){           \
        if(!Stack_Token_IsEmpty(stack)){                                \
            *data = stack->array[stack->topIndex];                      \
        }                                                               \
        else{                                                           \
            err_call(ERR_INTERNAL, NULL);                               \
        }                                                               \
    }                                                                   \
    void Stack_##TNAME##_Pop(Stack_##TNAME *stack ) {                   \
        if(!Stack_Token_IsEmpty(stack)){                                \
            stack->topIndex--;                                          \
        }                                                               \
    }                                                                   \
    void Stack_##TNAME##_Push(Stack_##TNAME *stack, T token ) {         \
        if(!Stack_Token_IsFull(stack)){                                 \
            stack->array[++stack->topIndex] = token;                    \
        }                                                               \
        else{                                                           \
            err_call(ERR_INTERNAL, NULL);                               \
        }                                                               \
    }

STACKDEC(token_ptr, Token)
STACKDEC(LocalBSTNodePtr, Bst)