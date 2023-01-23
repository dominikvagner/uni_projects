/**
 *  Project: Implementation of imperative language compiler IFJ21
 *  @file symtable.c
 *
 *  @brief File that implements symbol table using BST
 *
 *  @author <xpoliv06> Tomáš Polívka
 */

#include "symtable.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

void local_bst_init(LocalBSTNodePtr *tree)
{
    (*tree) = NULL;
}

void local_bst_insert(LocalBSTNodePtr *tree, char *key, token_type type)
{
    if((*tree) == NULL)
    {
        *tree = (LocalBSTNodePtr) malloc(sizeof(struct LocalBSTNode));

        if(*tree != NULL)
        {
            (*tree)->key = malloc(sizeof(char) * strlen(key));
            strcpy((*tree)->key, key);
            (*tree)->type = type;
            (*tree)->left = NULL;
            (*tree)->right = NULL;
        }

        return;
    }

    if(strcmp(key, (*tree)->key) < 0)
    {
        local_bst_insert(&(*tree)->left, key, type);
    }
    else
    {
        if(strcmp((*tree)->key, key) < 0)
        {
            local_bst_insert(&(*tree)->right, key, type);
        }
        else
        {
            (*tree)->type = type;
        }
    }
}

bool local_bst_search(LocalBSTNodePtr tree, char *key, LocalBSTNodePtr *pointer)
{
    if(tree == NULL || tree->key == NULL)
    {
        return false;
    }

    if(strcmp(tree->key, key) == 0)
    {
        if(pointer != NULL)
        {
            *pointer = tree;
        }
        return true;
    }
    else
    {
        if(strcmp(tree->key, key) > 0)
        {
            return local_bst_search(tree->left, key, pointer);
        }
        else
        {
            return local_bst_search(tree->right, key, pointer);
        }
    }
}

void local_bst_dispose(LocalBSTNodePtr *tree)
{
    if(*tree != NULL)
    {
        local_bst_dispose(&(*tree)->left);
        local_bst_dispose(&(*tree)->right);
        free(*tree);
    }
    (*tree) = NULL;
}

void global_bst_init(GlobalBSTNodePtr *tree)
{
    *tree = NULL;
}

bool global_bst_search(GlobalBSTNodePtr tree, char *key, functionPtrData *data)
{
    if(tree == NULL)
    {
        return false;
    }

    if(strcmp(tree->key, key) == 0)
    {
        if(data != NULL){
            *data = tree->content;
        }
        return true;
    }
    else
    {
        if(strcmp(tree->key, key) > 0)
        {
            return global_bst_search(tree->left, key, data);
        }
        else
        {
            return global_bst_search(tree->right, key, data);
        }
    }
}

void global_bst_insert(GlobalBSTNodePtr *tree, char *key, functionPtrData function_data)
{
    if((*tree) == NULL)
    {
        *tree = (GlobalBSTNodePtr) malloc(sizeof(struct GlobalBSTNode));

        if(*tree != NULL)
        {
            (*tree)->key = malloc(sizeof(char) * strlen(key));
            strcpy((*tree)->key, key);
            (*tree)->content = function_data;
            (*tree)->left = NULL;
            (*tree)->right = NULL;
        }

        return;
    }

    if(strcmp(key, (*tree)->key) < 0)
    {
        global_bst_insert(&(*tree)->left, key, function_data);
    }
    else
    {
        if(strcmp((*tree)->key, key) < 0)
        {
            global_bst_insert(&(*tree)->right, key, function_data);
        }
        else
        {
            (*tree)->content = function_data;
        }
    }
}

void global_bst_dispose(GlobalBSTNodePtr *tree)
{
    if(*tree != NULL)
    {
        global_bst_dispose(&(*tree)->left);
        global_bst_dispose(&(*tree)->right);
        free(*tree);
    }
    (*tree) = NULL;
}
