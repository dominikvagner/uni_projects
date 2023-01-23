/**
 *  Project: Implementation of imperative language compiler IFJ21
 *  @file stack.h
 *
 *  @brief Header file for stack.c.
 *
 *  @author <xhajek51> Vojtěch Hájek
 */

#ifndef IFJ_STACK_H
#define IFJ_STACK_H
#define MAX_STACK 50

#include "tokenList.h"
#include "symtable.h"

#define STACKDEC(T, TNAME)                                      \
    /**                                                         \
    * @brief stack struct.                                      \
    */                                                          \
    typedef struct {                                            \
        T array[MAX_STACK];                                     \
        int topIndex;                                           \
    } Stack_##TNAME;                                            \
                                                                \
    /**                                                         \
    * @brief Initialize of stack                                \
    * @param stack Pointer to the stack struct                  \
    */                                                          \
    void Stack_##TNAME##_Init(Stack_##TNAME *stack );           \
    /**                                                         \
     * @brief Returns token from stack top                      \
     * @param list Pointer to the stack struct                  \
     * @param dataPtr Pointer to target variable                \
     */                                                         \
    void Stack_##TNAME##_Top(Stack_##TNAME *stack, T *data);    \
    /**                                                         \
     * @brief Removes value from top of the stack               \
     * @param list Pointer to the stack struct                  \
     */                                                         \
    void Stack_##TNAME##_Pop(Stack_##TNAME*stack );             \
    /**                                                         \
     * @brief Returns not null value when stack is empty        \
     * @param list Pointer to the stack struct                  \
     */                                                         \
    int Stack_##TNAME##_IsEmpty(Stack_##TNAME *stack );         \
    /**                                                         \
     * @brief Return not null value when stack is full          \
     * @param list Pointer to the stack struct                  \
     */                                                         \
    int Stack_##TNAME##_IsFull(Stack_##TNAME *stack );          \
    /**                                                         \
     * @brief Adds value to top of the stack                    \
     * @param list Pointer to the stack struct                  \
     * @param token Value to insert                             \
     */                                                         \
    void Stack_##TNAME##_Push(Stack_##TNAME *stack, T data);

STACKDEC(token_ptr, Token)
STACKDEC(LocalBSTNodePtr, Bst)


#endif
