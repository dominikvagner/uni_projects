/*
 * Tabuľka s rozptýlenými položkami
 *
 * S využitím dátových typov zo súboru hashtable.h a pripravených kostier
 * funkcií implementujte tabuľku s rozptýlenými položkami s explicitne
 * zreťazenými synonymami.
 *
 * Pri implementácii uvažujte veľkosť tabuľky HT_SIZE.
 *
 * Funkce doplnil: Dominik Vágner, xvagne10
 */

#include "hashtable.h"
#include <stdlib.h>
#include <string.h>

int HT_SIZE = MAX_HT_SIZE;

/*
 * Rozptyľovacia funkcia ktorá pridelí zadanému kľúču index z intervalu
 * <0,HT_SIZE-1>. Ideálna rozptyľovacia funkcia by mala rozprestrieť kľúče
 * rovnomerne po všetkých indexoch. Zamyslite sa nad kvalitou zvolenej funkcie.
 */
int get_hash(char *key) {
  int result = 1;
  int length = strlen(key);
  for (int i = 0; i < length; i++) {
    result += key[i];
  }
  return (result % HT_SIZE);
}

/*
 * Inicializácia tabuľky — zavolá sa pred prvým použitím tabuľky.
 */
void ht_init(ht_table_t *table) {
  // Tabulka musí mít alespoň jednu položku
  if (HT_SIZE < 1)
    return;
  // Každou položku nastavíme na NULL
  for (int i = 0; i < HT_SIZE; i++) {
    (*table)[i] = NULL;
  }
}

/*
 * Vyhľadanie prvku v tabuľke.
 *
 * V prípade úspechu vráti ukazovateľ na nájdený prvok; v opačnom prípade vráti
 * hodnotu NULL.
 */
ht_item_t *ht_search(ht_table_t *table, char *key) {
  // Argumenty funkce nesmí být NULL
  if (key == NULL || table == NULL)
    return NULL;

  // Položka z tabulky podle hashe
  ht_item_t *item = (*table)[get_hash(key)];

  // Hledáme prvek se stejným klíčem jako argument funkce
  while (item != NULL) {
    if (!strcmp(item->key, key)) {
      break;
    }
    item = item->next;
  }
  return item;
}

/*
 * Vloženie nového prvku do tabuľky.
 *
 * Pokiaľ prvok s daným kľúčom už v tabuľke existuje, nahraďte jeho hodnotu.
 *
 * Pri implementácii využite funkciu ht_search. Pri vkladaní prvku do zoznamu
 * synonym zvoľte najefektívnejšiu možnosť a vložte prvok na začiatok zoznamu.
 */
void ht_insert(ht_table_t *table, char *key, float value) {
  // Vyhledání položky, pokud existuje tak nahradit value
  ht_item_t *existing = ht_search(table, key);
  if (existing != NULL) {
    existing->value = value;
    return;
  }

  // Malloc nové položky
  ht_item_t *new = malloc(sizeof(ht_item_t) * 1);
  if (new == NULL)
    return;

  // Nastavení nové položky, a vložení na začátek
  new->key = key;
  new->value = value;
  new->next = (*table)[get_hash(key)];
  (*table)[get_hash(key)] = new;
  return;
}

/*
 * Získanie hodnoty z tabuľky.
 *
 * V prípade úspechu vráti funkcia ukazovateľ na hodnotu prvku, v opačnom
 * prípade hodnotu NULL.
 *
 * Pri implementácii využite funkciu ht_search.
 */
float *ht_get(ht_table_t *table, char *key) {
  ht_item_t *existing = ht_search(table, key);
  if (existing != NULL)
    return &existing->value;

  return NULL;
}

/*
 * Zmazanie prvku z tabuľky.
 *
 * Funkcia korektne uvoľní všetky alokované zdroje priradené k danému prvku.
 * Pokiaľ prvok neexistuje, nerobte nič.
 *
 * Pri implementácii NEVYUŽÍVAJTE funkciu ht_search.
 */
void ht_delete(ht_table_t *table, char *key) {
  ht_item_t *tmp = (*table)[get_hash(key)];
  ht_item_t *prev;

  // Pokud položka v tabulce má jen jeden prvek
  if (tmp->next == NULL) {
    free(tmp);
    return;
  }

  // Pokud prvek který máme smazat je první v položce tabulky
  if (tmp != NULL && !strcmp(tmp->key, key)) {
    prev = tmp;
    tmp = tmp->next;
    free(prev);
    return;
  }

  // Nalezení prvku v položce tabulky
  while (tmp != NULL && strcmp(tmp->key, key)) {
    prev = tmp;
    tmp = tmp->next;
  }

  // Pokud prvek neexistuje
  if (tmp == NULL) {
    return;
  }

  // Smazaní prvku
  prev->next = tmp->next;
  free(tmp);
}

/*
 * Zmazanie všetkých prvkov z tabuľky.
 *
 * Funkcia korektne uvoľní všetky alokované zdroje a uvedie tabuľku do stavu
 * po inicializácii.
 */
void ht_delete_all(ht_table_t *table) {
  // Cyklus všemi položkami tabulky
  for (int i = 0; i < HT_SIZE; i++) {
    // Kontrola na už prázdnou položku
    if ((*table)[i] == NULL)
      continue;

    // Smazaní všech prvků položky
    ht_item_t *item = (*table)[i];
    ht_item_t *tmp;
    while (item != NULL) {
      tmp = item;
      item = item->next;
      free(tmp);
    }
    // Nastavení položky na NULL
    (*table)[i] = NULL;
  }
}
