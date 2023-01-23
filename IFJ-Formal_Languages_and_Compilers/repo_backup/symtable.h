/**
 *  Project: Implementation of imperative language compiler IFJ21
 *  @file symtable.h
 *
 *  @brief Header file for symtable.c
 *
 *  @author <xpoliv06> Tomáš Polívka
 */

#ifndef IFJ_SYMTABLE_H
#define IFJ_SYMTABLE_H

#include <stdbool.h>
#include "tokenList.h"
#define MAXPARAMS 256

/**
 * @brief Struct for node of local tree
 */
typedef struct LocalBSTNode {
    char *key;
    token_type type;            // only T_K_NUMBER, T_K_INT, T_K_STRING, T_K_NILL
    struct LocalBSTNode *left;
    struct LocalBSTNode *right;
} *LocalBSTNodePtr;

/**
 * @brief Struct for data of function
 */
typedef struct functionData{
    int numOfParams;
    int numOfReturns;
    char *params[MAXPARAMS];
    char *key;
    token_type paramsType[MAXPARAMS];
    token_type returnsType[MAXPARAMS];
} *functionPtrData;

/**
 * @brief Struct for node of global tree
 */
typedef struct GlobalBSTNode {
    char *key;
    struct functionData *content;
    struct GlobalBSTNode *left;
    struct GlobalBSTNode *right;
} *GlobalBSTNodePtr;

// Function for LOCAL_BST

/**
 * @brief
 *
 * @param tree
 */
void local_bst_init(LocalBSTNodePtr *tree);

/**
 * @brief
 *
 * @param tree
 * @param key
 * @param type
 */
void local_bst_insert(LocalBSTNodePtr *tree, char *key, token_type type);

/**
 * @brief
 *
 * @param tree
 * @param key
 */
bool local_bst_search(LocalBSTNodePtr tree, char *key, LocalBSTNodePtr *pointer);

/**
 * @brief
 *
 * @param tree
 */
void local_bst_dispose(LocalBSTNodePtr *tree);

// Function for GLOBAL_BST

/**
 * @brief
 *
 * @param tree
 */
void global_bst_init(GlobalBSTNodePtr *tree);

/**
 * @brief
 *
 * @param tree
 * @param key
 */
bool global_bst_search(GlobalBSTNodePtr tree, char *key, functionPtrData *data);

/**
 * @brief
 *
 * @param tree
 * @param key
 * @param function_data
 */
void global_bst_insert(GlobalBSTNodePtr *tree, char *key, functionPtrData function_data);

/**
 * @brief
 *
 * @param tree
 */
void global_bst_dispose(GlobalBSTNodePtr *tree);

#endif
