//======== Copyright (c) 2021, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     Test Driven Development - priority queue code
//
// $NoKeywords: $ivs_project_1 $tdd_code.cpp
// $Author:     Dominik Vágner <xvagne10@stud.fit.vutbr.cz>
// $Date:       $2021-03-09
//============================================================================//
/**
 * @file tdd_code.cpp
 * @author Dominik Vágner
 * 
 * @brief Implementace metod tridy prioritni fronty.
 */

#include <stdlib.h>
#include <stdio.h>

#include "tdd_code.h"

//============================================================================//
// ** ZDE DOPLNTE IMPLEMENTACI **
//
// Zde doplnte implementaci verejneho rozhrani prioritni fronty (Priority Queue)
// 1. Verejne rozhrani fronty specifikovane v: tdd_code.h (sekce "public:")
//    - Konstruktor (PriorityQueue()), Destruktor (~PriorityQueue())
//    - Metody Insert/Remove/Find a GetHead
//    - Pripadne vase metody definovane v tdd_code.h (sekce "protected:")
//
// Cilem je dosahnout plne funkcni implementace prioritni fronty implementovane
// pomoci tzv. "double-linked list", ktera bude splnovat dodane testy 
// (tdd_tests.cpp).
//============================================================================//

PriorityQueue::PriorityQueue()
{

}

PriorityQueue::~PriorityQueue()
{
    auto tmp = m_pHead;
    while(m_pHead != nullptr) {
        tmp = m_pHead;
        m_pHead = m_pHead->pNext;
        delete tmp;
    }
    m_pHead = nullptr;
}

void PriorityQueue::Insert(int value)
{
    // If list is empty add first node
    if (m_pHead == nullptr) {
        m_pHead = new Element_t {
            .pNext = nullptr,
            .value = value
        };
        return;
    }

    auto tmp = m_pHead;

    // If value is bigger than head then set new head
    if (value > m_pHead->value) {
        m_pHead = new Element_t {
                .pNext = tmp,
                .value = value
        };
        return;
    }

    // Go through nodes and place new node where its supposed to be, MAX -> MIN
    while ((tmp->pNext != nullptr) && (value < tmp->pNext->value)) {
        tmp = tmp->pNext;
    }

    auto next = tmp->pNext;
    tmp->pNext = new Element_t {
        .pNext = next,
        .value = value
    };
}

bool PriorityQueue::Remove(int value)
{
    Element_t *tmp = m_pHead;
    Element_t *prev = nullptr;

    if ((tmp != nullptr) && (tmp->value == value)) {
        m_pHead = tmp->pNext;
        delete tmp;
        return true;
    } else {
        while ((tmp != nullptr) && (tmp->value != value)) {
            prev = tmp;
            tmp = tmp->pNext;
        }

        if(tmp == nullptr)
            return false;

        prev->pNext = tmp->pNext;
        delete tmp;
        return true;
    }
}

PriorityQueue::Element_t *PriorityQueue::Find(int value)
{
    Element_t *tmp = m_pHead;

    while ((tmp != nullptr) && (tmp->value != value)) {
        tmp = tmp->pNext;
    }

    if(tmp == nullptr)
        return nullptr;

    return tmp;
}

size_t PriorityQueue::Length()
{
    Element_t *tmp = m_pHead;
    int count = 0;

    if(tmp == nullptr)
        return 0;

    while (tmp != nullptr) {
        tmp = tmp->pNext;
        count++;
    }

    return count;
}

PriorityQueue::Element_t *PriorityQueue::GetHead()
{
    return m_pHead;
}

/*** Konec souboru tdd_code.cpp ***/
