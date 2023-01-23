/**
 *  Project: Implementation of imperative language compiler IFJ21
 *  @file tokenList.c
 *
 *  @brief Implementation doubly linked list.
 *
 *  @author <xpoliv06> Tomáš Polívka
 *  @author <xhajek51> Vojtěch Hájek
 */

#include "tokenList.h"
#include "error.h"

void DLL_Init(DLList *list) {
  list->first = NULL;
  list->last = NULL;
  list->active = NULL;
} /*DLL_Init*/

void DLL_Dispose(DLList *list) {
  if (list->first == NULL) {
    return;
  }

  token_ptr tempPtr;

  while (list->first != NULL) {
    tempPtr = list->first;
    list->first = tempPtr->next;
    free(tempPtr);
  }

  DLL_Init(list);
} /*DLL_Dispose*/

void DLL_InsertFirst(DLList *list, token_ptr token) {
  token_ptr new_token;
  new_token = token;
  new_token->prev = NULL;
  new_token->next = list->first;
  if (list->first != NULL) {
    list->first->prev = new_token;
  } else {
    list->last = new_token;
  }
  list->first = new_token;
} /*DLL_InsertFirst*/

void DLL_InsertLast(DLList *list, token_ptr token) {
  token_ptr new_token;
  new_token = token;
  new_token->next = NULL;
  new_token->prev = list->last;
  if (list->last != NULL) {
    list->last->next = new_token;
  } else {
    list->first = new_token;
  }
  list->last = new_token;
} /*DLL_InsertLast*/

void DLL_First(DLList *list) { list->active = list->first; } /*DLL_First*/

void DLL_Last(DLList *list) { list->active = list->last; } /*DLL_Last*/

void DLL_GetFirst(DLList *list, token_ptr *token) {
  if (list->first != NULL) {
    *token = list->first;
  } else {
    err_call(ERR_INTERNAL, NULL);
  }
} /*DLL_GetFirst*/

void DLL_GetLast(DLList *list, token_ptr *token) {
  if (list->first != NULL) {
    *token = list->last;
  } else {
    err_call(ERR_INTERNAL, NULL);
  }
} /*DLL_GetLast*/

void DLL_DeleteFirst(DLList *list) {
  if (list->first != NULL) {
    token_ptr token;
    token = list->first;
    if (list->active == list->first) {
      list->active = NULL;
    }
    if (list->first == list->last) {
      list->first = NULL;
      list->last = NULL;
    } else {
      list->first = list->first->next;
      list->first->prev = NULL;
    }
    free(token);
  }
} /*DLL_DeleteFirst*/

void DLL_DeleteLast(DLList *list) {
  if (list->first == NULL) {
    return;
  }

  token_ptr token;
  token = list->last;
  if (list->active == list->last) {
    list->active = NULL;
  }
  if (list->last == list->first) {

    list->last = NULL;
    list->first = NULL;
  } else {
    list->last = list->last->prev;
    list->last->next = NULL;
  }
  free(token);
} /*DLL_DeleteLast*/

void DLL_DeleteAfter(DLList *list) {
  if (list->active == NULL || list->active == list->last) {
    return;
  }

  token_ptr token;
  token = list->active->next;
  list->active->next = token->next;
  if (token == list->last) {
    list->last = list->active;
  } else {
    token->next->prev = list->active;
  }
  free(token);
} /*DLL_DeleteAfter */

void DLL_DeleteBefore(DLList *list) {
  if (list->active == NULL || list->active == list->first) {
    return;
  }

  token_ptr token;
  token = list->active->prev;
  list->active->prev = token->prev;
  if (token == list->first) {
    list->first = list->active;
  } else {
    token->prev->next = list->active;
  }
  free(token);

} /*DLL_DeleteBefore*/

void DLL_InsertAfter(DLList *list, token_ptr token) {
  if (list->active == NULL) {
    return;
  }

  token_ptr new_token;

  new_token = token;
  new_token->next = list->active->next;
  new_token->prev = list->active;
  list->active->next = new_token;

  if (list->active == list->last) {
    list->last = new_token;
  } else {
    new_token->next->prev = new_token;
  }

} /*DLL_InsertAfter */

void DLL_InsertBefore(DLList *list, token_ptr token) {
  if (list->active == NULL) {
    return;
  }

  token_ptr new_token;
  new_token = token;
  new_token->prev = list->active->prev;
  new_token->next = list->active;
  list->active->prev = new_token;

  if (list->active == list->first) {
    list->first = new_token;
  } else {
    new_token->prev->next = new_token;
  }
} /*DLL_InsertBefore*/

void DLL_GetValue(DLList *list, token_ptr *token) {
  if (list->active != NULL) {
    *token = list->active;
  } else {
    err_call(ERR_INTERNAL, NULL);
  }
} /*DLL_GetValue*/

void DLL_SetValue(DLList *list, token_ptr token) {
  if (list->active != NULL) {
    list->active = token;
  }
} /*DLL_SetValue*/

void DLL_Next(DLList *list) {
  if (list->active != NULL) {
    list->active = list->active->next;
  }
} /*DLL_Next*/

void DLL_Previous(DLList *list) {
  if (list->active != NULL) {
    list->active = list->active->prev;
  }
} /*DLL_Previous*/

int DLL_IsActive(DLList *list) {
  return (list->active != NULL);
} /*DLL_IsActive*/