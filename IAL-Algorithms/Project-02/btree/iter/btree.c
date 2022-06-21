/*
 * Binárny vyhľadávací strom — iteratívna varianta
 *
 * S využitím dátových typov zo súboru btree.h, zásobníkov zo súborov stack.h a
 * stack.c a pripravených kostier funkcií implementujte binárny vyhľadávací
 * strom bez použitia rekurzie.
 *
 * Funkce doplnil: Dominik Vágner, xvagne10
 */

#include "../btree.h"
#include "stack.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * Inicializácia stromu.
 *
 * Užívateľ musí zaistiť, že incializácia sa nebude opakovane volať nad
 * inicializovaným stromom. V opačnom prípade môže dôjsť k úniku pamäte (memory
 * leak). Keďže neinicializovaný ukazovateľ má nedefinovanú hodnotu, nie je
 * možné toto detegovať vo funkcii.
 */
void bst_init(bst_node_t **tree) { (*tree) = NULL; }

/*
 * Nájdenie uzlu v strome.
 *
 * V prípade úspechu vráti funkcia hodnotu true a do premennej value zapíše
 * hodnotu daného uzlu. V opačnom prípade funckia vráti hodnotu false a premenná
 * value ostáva nezmenená.
 *
 * Funkciu implementujte iteratívne bez použitia vlastných pomocných funkcií.
 */
bool bst_search(bst_node_t *tree, char key, int *value) {
  while (tree != NULL) {
    if (key < tree->key) {
      tree = tree->left;
    } else if (key > tree->key) {
      tree = tree->right;
    } else {
      *value = tree->value;
      return true;
    }
  }
  return false;
}

/*
 * Vloženie uzlu do stromu.
 *
 * Pokiaľ uzol so zadaným kľúčom v strome už existuje, nahraďte jeho hodnotu.
 * Inak vložte nový listový uzol.
 *
 * Výsledný strom musí spĺňať podmienku vyhľadávacieho stromu — ľavý podstrom
 * uzlu obsahuje iba menšie kľúče, pravý väčšie.
 *
 * Funkciu implementujte iteratívne bez použitia vlastných pomocných funkcií.
 */
void bst_insert(bst_node_t **tree, char key, int value) {
  bst_node_t *tmp = *tree;
  bst_node_t *prev = NULL;
  bst_node_t *insert = malloc(sizeof(bst_node_t));
  insert->key = key;
  insert->value = value;
  insert->left = insert->right = NULL;

  while (tmp != NULL) {
    prev = tmp;
    if (key < tmp->key) {
      tmp = tmp->left;
    } else if (key > tmp->key) {
      tmp = tmp->right;
    } else {
      tmp->value = value;
      free(insert);
      return;
    }
  }
  if (prev == NULL) {
    prev = insert;
    *tree = prev;
  } else if (key < prev->key) {
    prev->left = insert;
  } else {
    prev->right = insert;
  }
}

/*
 * Pomocná funkcia ktorá nahradí uzol najpravejším potomkom.
 *
 * Kľúč a hodnota uzlu target budú nahradené kľúčom a hodnotou najpravejšieho
 * uzlu podstromu tree. Najpravejší potomok bude odstránený. Funkcia korektne
 * uvoľní všetky alokované zdroje odstráneného uzlu.
 *
 * Funkcia predpokladá že hodnota tree nie je NULL.
 *
 * Táto pomocná funkcia bude využitá pri implementácii funkcie bst_delete.
 *
 * Funkciu implementujte iteratívne bez použitia vlastných pomocných funkcií.
 */
void bst_replace_by_rightmost(bst_node_t *target, bst_node_t **tree) {
  bst_node_t *parent_rm = *tree;
  bst_node_t *rm = (*tree)->left;

  while (rm->right != NULL) {
    parent_rm = rm;
    rm = rm->right;
  }

  target->key = rm->key;
  target->value = rm->value;

  if (parent_rm->right == rm) {
    parent_rm->right = rm->left;
  } else {
    parent_rm->left = rm->left;
  }

  free(rm);
}

/*
 * Odstránenie uzlu v strome.
 *
 * Pokiaľ uzol so zadaným kľúčom neexistuje, funkcia nič nerobí.
 * Pokiaľ má odstránený uzol jeden podstrom, zdedí ho otec odstráneného uzla.
 * Pokiaľ má odstránený uzol oba podstromy, je nahradený najpravejším uzlom
 * ľavého podstromu. Najpravejší uzol nemusí byť listom!
 * Funkcia korektne uvoľní všetky alokované zdroje odstráneného uzlu.
 *
 * Funkciu implementujte iteratívne pomocou bst_replace_by_rightmost a bez
 * použitia vlastných pomocných funkcií.
 */
void bst_delete(bst_node_t **tree, char key) {
  bst_node_t *tmp = *tree;
  bst_node_t *prev = NULL;

  while (tmp != NULL) {
    if (key < tmp->key) {
      prev = tmp;
      tmp = tmp->left;
    } else if (key > tmp->key) {
      prev = tmp;
      tmp = tmp->right;
    } else {
      break;
    }
  }

  if (tmp == NULL) {
    return;
  }

  if (tmp->left == NULL) {
    if (prev == NULL) {
      (*tree) = tmp->right;
    } else {
      if (key < prev->key) {
        prev->left = tmp->right;
      } else {
        prev->right = tmp->right;
      }
    }
    free(tmp);
  } else {
    bst_replace_by_rightmost(tmp, &tmp);
  }
}

/*
 * Zrušenie celého stromu.
 *
 * Po zrušení sa celý strom bude nachádzať v rovnakom stave ako po
 * inicializácii. Funkcia korektne uvoľní všetky alokované zdroje rušených
 * uzlov.
 *
 * Funkciu implementujte iteratívne pomocou zásobníku uzlov a bez použitia
 * vlastných pomocných funkcií.
 */
void bst_dispose(bst_node_t **tree) {
  if (*tree == NULL) {
    return;
  }

  stack_bst_t s1;
  stack_bst_init(&s1);

  do {
    if (*tree == NULL) {
      if (!stack_bst_empty(&s1)) {
        *tree = stack_bst_top(&s1);
        stack_bst_pop(&s1);
      }
    } else {
      if ((*tree)->right != NULL) {
        stack_bst_push(&s1, (*tree)->right);
      }
      bst_node_t *tmp = *tree;
      *tree = (*tree)->left;
      free(tmp);
    }
  } while (*tree != NULL || !stack_bst_empty(&s1));
}

/*
 * Pomocná funkcia pre iteratívny preorder.
 *
 * Prechádza po ľavej vetve k najľavejšiemu uzlu podstromu.
 * Nad spracovanými uzlami zavola bst_print_node a uloží ich do zásobníku
 * uzlov.
 *
 * Funkciu implementujte iteratívne pomocou zásobníku uzlov a bez použitia
 * vlastných pomocných funkcií.
 */
void bst_leftmost_preorder(bst_node_t *tree, stack_bst_t *to_visit) {
  while (tree != NULL) {
    stack_bst_push(to_visit, tree);
    bst_print_node(tree);
    tree = tree->left;
  }
}

/*
 * Preorder prechod stromom.
 *
 * Pre aktuálne spracovávaný uzol nad ním zavolajte funkciu bst_print_node.
 *
 * Funkciu implementujte iteratívne pomocou funkcie bst_leftmost_preorder a
 * zásobníku uzlov bez použitia vlastných pomocných funkcií.
 */
void bst_preorder(bst_node_t *tree) {
  stack_bst_t s1;
  stack_bst_init(&s1);

  bst_leftmost_preorder(tree, &s1);

  while (!stack_bst_empty(&s1)) {
    tree = stack_bst_top(&s1);
    stack_bst_pop(&s1);
    bst_leftmost_preorder(tree->right, &s1);
  }
}

/*
 * Pomocná funkcia pre iteratívny inorder.
 *
 * Prechádza po ľavej vetve k najľavejšiemu uzlu podstromu a ukladá uzly do
 * zásobníku uzlov.
 *
 * Funkciu implementujte iteratívne pomocou zásobníku uzlov a bez použitia
 * vlastných pomocných funkcií.
 */
void bst_leftmost_inorder(bst_node_t *tree, stack_bst_t *to_visit) {
  while (tree != NULL) {
    stack_bst_push(to_visit, tree);
    tree = tree->left;
  }
}

/*
 * Inorder prechod stromom.
 *
 * Pre aktuálne spracovávaný uzol nad ním zavolajte funkciu bst_print_node.
 *
 * Funkciu implementujte iteratívne pomocou funkcie bst_leftmost_inorder a
 * zásobníku uzlov bez použitia vlastných pomocných funkcií.
 */
void bst_inorder(bst_node_t *tree) {
  stack_bst_t s1;
  stack_bst_init(&s1);

  bst_leftmost_inorder(tree, &s1);

  while (!stack_bst_empty(&s1)) {
    tree = stack_bst_top(&s1);
    stack_bst_pop(&s1);
    bst_print_node(tree);
    bst_leftmost_inorder(tree->right, &s1);
  }
}

/*
 * Pomocná funkcia pre iteratívny postorder.
 *
 * Prechádza po ľavej vetve k najľavejšiemu uzlu podstromu a ukladá uzly do
 * zásobníku uzlov. Do zásobníku bool hodnôt ukladá informáciu že uzol
 * bol navštívený prvý krát.
 *
 * Funkciu implementujte iteratívne pomocou zásobníkov uzlov a bool hodnôt a
 * bez použitia vlastných pomocných funkcií.
 */
void bst_leftmost_postorder(bst_node_t *tree, stack_bst_t *to_visit,
                            stack_bool_t *first_visit) {
  while (tree != NULL) {
    stack_bst_push(to_visit, tree);
    stack_bool_push(first_visit, true);
    tree = tree->left;
  }
}

/*
 * Postorder prechod stromom.
 *
 * Pre aktuálne spracovávaný uzol nad ním zavolajte funkciu bst_print_node.
 *
 * Funkciu implementujte iteratívne pomocou funkcie bst_leftmost_postorder a
 * zásobníkov uzlov a bool hodnôt bez použitia vlastných pomocných funkcií.
 */
void bst_postorder(bst_node_t *tree) {
  bool from_left;
  stack_bst_t s1;
  stack_bst_init(&s1);
  stack_bool_t s2;
  stack_bool_init(&s2);

  bst_leftmost_postorder(tree, &s1, &s2);

  while (!stack_bst_empty(&s1)) {
    tree = stack_bst_top(&s1);
    from_left = stack_bool_top(&s2);
    stack_bool_pop(&s2);
    if (from_left) {
      stack_bool_push(&s2, false);
      bst_leftmost_postorder(tree->right, &s1, &s2);
    } else {
      stack_bst_pop(&s1);
      bst_print_node(tree);
    }
  }
}
