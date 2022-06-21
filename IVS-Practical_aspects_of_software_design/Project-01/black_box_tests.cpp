//======== Copyright (c) 2021, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     Red-Black Tree - public interface tests
//
// $NoKeywords: $ivs_project_1 $black_box_tests.cpp
// $Author:     Dominik Vágner <xvagne10@stud.fit.vutbr.cz>
// $Date:       $2021-03-09
//============================================================================//
/**
 * @file black_box_tests.cpp
 * @author Dominik Vágner
 * 
 * @brief Implementace testu binarniho stromu.
 */

#include <vector>

#include "gtest/gtest.h"

#include "red_black_tree.h"

//============================================================================//
// ** ZDE DOPLNTE TESTY **
//
// Zde doplnte testy Red-Black Tree, testujte nasledujici:
// 1. Verejne rozhrani stromu
//    - InsertNode/DeleteNode a FindNode
//    - Chovani techto metod testuje pro prazdny i neprazdny strom.
// 2. Axiomy (tedy vzdy platne vlastnosti) Red-Black Tree:
//    - Vsechny listove uzly stromu jsou *VZDY* cerne.
//    - Kazdy cerveny uzel muze mit *POUZE* cerne potomky.
//    - Vsechny cesty od kazdeho listoveho uzlu ke koreni stromu obsahuji
//      *STEJNY* pocet cernych uzlu.
//============================================================================//

class NonEmptyTree : public ::testing::Test
{
protected:
    virtual void SetUp() {
        int values[] = { 2, 1, 5 };

        for(int i = 0; i < 3; ++i)
            tree.InsertNode(values[i]);
    }

    BinaryTree tree;
};

class EmptyTree : public ::testing::Test
{
protected:
    BinaryTree tree;
};

class TreeAxioms : public ::testing::Test
{
protected:
    virtual void SetUp() {
        int values[] = { 2, 1, 5 };

        for(int i = 0; i < 3; ++i)
            tree.InsertNode(values[i]);
    }

    BinaryTree tree;
};

TEST_F(NonEmptyTree, InsertNode)
{
    auto insert_return = tree.InsertNode(10);
    ASSERT_TRUE(insert_return.first);
    EXPECT_EQ(tree.GetRoot()->pRight->pRight->key, 10);

    insert_return = tree.InsertNode(10);
    ASSERT_FALSE(insert_return.first);
}

TEST_F(NonEmptyTree, DeleteNode)
{
    ASSERT_FALSE(tree.DeleteNode(10));

    ASSERT_TRUE(tree.DeleteNode(5));
    ASSERT_TRUE(tree.DeleteNode(1));
    ASSERT_TRUE(tree.DeleteNode(2));

    EXPECT_EQ(tree.GetRoot(), nullptr);
}

TEST_F(NonEmptyTree, FindNode)
{
    int values[] = { 2, 1, 5 };

    for (int i = 0; i < 3; ++i) {
        BinaryTree::Node_t *node = tree.FindNode(values[i]);
        ASSERT_TRUE(node != nullptr);
        EXPECT_EQ(node->key, values[i]);
    }

    EXPECT_EQ(tree.FindNode(10), nullptr);
}

TEST_F(EmptyTree, InsertNode)
{
    EXPECT_TRUE(tree.GetRoot() == NULL);

    tree.InsertNode(0);
    ASSERT_TRUE(tree.GetRoot() != NULL);
    EXPECT_EQ(tree.GetRoot()->key, 0);
    EXPECT_EQ(tree.GetRoot()->color, 1);

    tree.InsertNode(100);
    ASSERT_TRUE(tree.GetRoot() != NULL);
    EXPECT_EQ(tree.GetRoot()->pRight->key, 100);
}

TEST_F(EmptyTree, DeleteNode)
{
    EXPECT_FALSE(tree.DeleteNode(0));
}

TEST_F(EmptyTree, FindNode)
{
    EXPECT_EQ(tree.FindNode(0), nullptr);
}

TEST_F(TreeAxioms, Axiom1)
{
    std::vector<Node_t *> leaf_nodes;
    tree.GetLeafNodes(leaf_nodes);

    int len = leaf_nodes.size();

    for (int i = 0; i < len; i++) {
        ASSERT_TRUE(leaf_nodes[i]->color == 1);
    }

    for (int i = 0; i < len; i++) {
        ASSERT_FALSE(leaf_nodes[i]->color == 0);
    }
}

TEST_F(TreeAxioms, Axiom2)
{
    std::vector<Node_t *> non_leaf_nodes;
    tree.GetNonLeafNodes(non_leaf_nodes);

    int len = non_leaf_nodes.size();

    for (int i = 0; i < len; i++) {
        if (non_leaf_nodes[i]->color == 1)
            non_leaf_nodes.erase(non_leaf_nodes.begin() + i);
    }

    len = non_leaf_nodes.size();

    for (int i = 0; i < len; i++) {
        ASSERT_TRUE(non_leaf_nodes[i]->pLeft->color == 1);
        ASSERT_TRUE(non_leaf_nodes[i]->pRight->color == 1);
    }
}

TEST_F(TreeAxioms, Axiom3)
{
    std::vector<Node_t *> leaf_nodes;
    tree.GetLeafNodes(leaf_nodes);

    int len = leaf_nodes.size();
    int black_node_counter[len];

    for (int i = 0; i < len; i++) {
        auto tmp = leaf_nodes[i];
        black_node_counter[i] = 0;

        while (tmp != nullptr) {
            if (tmp->color == 1)
                black_node_counter[i] += 1;
            tmp = tmp->pParent;
        }
    }

    int total_count = black_node_counter[0];

    for (int i = 0; i < len; i++) {
        EXPECT_EQ(total_count, black_node_counter[i]);
    }
}

/*** Konec souboru black_box_tests.cpp ***/
