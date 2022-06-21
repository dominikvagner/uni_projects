//======== Copyright (c) 2021, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     White Box - Tests suite
//
// $NoKeywords: $ivs_project_1 $white_box_code.cpp
// $Author:     Dominik Vágner <xvagne10@stud.fit.vutbr.cz>
// $Date:       $2021-03-09
//============================================================================//
/**
 * @file white_box_tests.cpp
 * @author Dominik Vágner
 * 
 * @brief Implementace testu prace s maticemi.
 */

#include "gtest/gtest.h"
#include "white_box_code.h"

//============================================================================//
// ** ZDE DOPLNTE TESTY **
//
// Zde doplnte testy operaci nad maticemi. Cilem testovani je:
// 1. Dosahnout maximalniho pokryti kodu (white_box_code.cpp) testy.
// 2. Overit spravne chovani operaci nad maticemi v zavislosti na rozmerech 
//    matic.
//============================================================================//

class DifferentSizeMatrices : public ::testing::Test
{
protected:
    Matrix m1x1{1, 1};
    Matrix m2x2{2, 2};
    Matrix m2x3{2, 3};
    Matrix m3x2{3, 2};
    Matrix m3x3{3, 3};
};

TEST(EmptyMatrix, MatrixConstructors)
{
    // Constructor without parameters
    Matrix m{};
    EXPECT_EQ(m.get(0, 0), 0);

    // Constructor with parameters
    Matrix m2{2, 2};
    EXPECT_NO_THROW(Matrix(3, 3));
    EXPECT_ANY_THROW(Matrix(3, 0));
    EXPECT_ANY_THROW(Matrix(0, 0));
    EXPECT_ANY_THROW(Matrix(-2, 2));
}

TEST_F(DifferentSizeMatrices, MatrixSet)
{
    // Set single value
    Matrix m{2, 2};
    m.set(1, 1, 42);
    EXPECT_EQ(m.get(1, 1), 42);
    EXPECT_FALSE(m.set(2, 2, 42));

    // Set whole matrix

    //2x3
    m2x3.set({ {1.2, 2.4, 4.8}, {1.3, 2.6, 5.2} });
    double values2x3[] = {1.2, 2.4, 4.8, 1.3, 2.6, 5.2};

    for (int i = 0; i < 2; i++) {
       for (int j = 0; j < 3; j++) {
            EXPECT_EQ(m2x3.get(i, j), values2x3[i * 3 + j]);
       }
    }

    //3x2
    m3x2.set({ {1.2, 2.4}, {1.3, 2.6}, {1.4, 2.8} });
    double values3x2[] = {1.2, 2.4, 1.3, 2.6, 1.4, 2.8};

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 2; j++) {
            EXPECT_EQ(m3x2.get(i, j), values3x2[i * 2 + j]);
        }
    }

    //3x3
    m3x3.set({ {1.2, 2.4, 4.8}, {1.3, 2.6, 5.2}, {1.4, 2.8, 5.6} });
    double values3x3[] = {1.2, 2.4, 4.8, 1.3, 2.6, 5.2, 1.4, 2.8, 5.6};

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            EXPECT_EQ(m3x3.get(i, j), values3x3[i * 3 + j]);
        }
    }

    // Set out of reach
    EXPECT_FALSE(m2x3.set({ {1.2, 2.4, 4.8, 9.6}, {1.3, 2.6, 5.2} }));
    EXPECT_FALSE(m2x3.set({ {1.2, 2.4, 4.8}, {1.3, 2.6, 5.2}, {1.4, 2.8, 5.6} }));
}

TEST_F(DifferentSizeMatrices, MatrixGet)
{
    EXPECT_NO_THROW(m3x3.get(1, 1));
    EXPECT_ANY_THROW(m3x3.get(2, 4));
    EXPECT_ANY_THROW(m3x3.get(4, 2));
}

TEST_F(DifferentSizeMatrices, OperatorEqual)
{
    Matrix m1{2, 3};
    EXPECT_TRUE(m2x3.operator==(m1));
    m1.set(1, 1, 42);
    EXPECT_FALSE(m2x3.operator==(m1));

    Matrix m2{3, 3};
    EXPECT_TRUE(m3x3.operator==(m2));

    EXPECT_ANY_THROW(m2x3.operator==(m3x3));
}

TEST_F(DifferentSizeMatrices, OperatorPlus)
{
    EXPECT_ANY_THROW(m2x3.operator+(m3x3));
    EXPECT_NO_THROW(m2x3.operator+(m2x3));

    m2x3.set({ {1, 2, 3}, {4, 5, 6} });
    Matrix m1{2, 3};
    m1.set({ {6, 5, 4}, {3, 2, 1} });
    Matrix m2{2, 3};
    m2.set({ {7, 7, 7}, {7, 7, 7} });

    EXPECT_EQ(m2x3.operator+(m1), m2);

    m3x3.set({ {1, 2, 3}, {4, 5, 6}, {7, 8, 9} });
    Matrix m3{3, 3};
    m3.set({ {10, 20, 30}, {6, 5, 4}, {3, 2, 1} });
    Matrix m4{3, 3};
    m4.set({ {11, 22, 33}, {10, 10, 10}, {10, 10, 10} });

    EXPECT_EQ(m3x3.operator+(m3), m4);
}

TEST_F(DifferentSizeMatrices, OperatorMultiplication)
{
    EXPECT_ANY_THROW(m3x2.operator*(m3x3));
    EXPECT_NO_THROW(m2x3.operator*(m3x3));
    EXPECT_NO_THROW(m3x3.operator*(m3x3));

    // 3x3 * 3x3
    m3x3.set({ {1, 12, 21}, {33, 22, 11}, {4, 6, 0} });
    Matrix m1{3, 3};
    m1.set({ {1, 5, 2}, {10, 0, 15}, {3, 20, 4}});
    Matrix m2{3, 3};
    m2.set({ {184, 425, 266}, {286, 385, 440}, {64, 20, 98} });

    EXPECT_EQ(m3x3.operator*(m1), m2);

    // 2x3 * 3x3
    m2x3.set({ {1, 2, 3}, {6, 5, 4}});
    Matrix m3{3, 3};
    m3.set({ {1, 4, 7}, {2, 5, 8}, {3, 6, 9}});
    Matrix m4{2, 3};
    m4.set({ {14, 32, 50}, {28, 73, 118} });

    EXPECT_EQ(m2x3.operator*(m3), m4);

    // 2x3 * 3x2
    m2x3.set({ {1, 2, 3}, {6, 5, 4}});
    m3x2.set({ {1, 4}, {2, 5}, {3, 6}});
    m2x2.set({ {14, 32}, {28, 73} });

    EXPECT_EQ(m2x3.operator*(m3x2), m2x2);
}

TEST_F(DifferentSizeMatrices, OperatorMultiplicationScalar)
{
    //2x3
    m2x3.set({ {1, 2, 3}, {6, 5, 4} });
    EXPECT_EQ(m2x3.operator*(1), m2x3);

    Matrix m1{2, 3};
    m1.set({ {-1, -2, -3}, {-6, -5, -4} });
    EXPECT_EQ(m2x3.operator*(-1), m1);

    Matrix m2{2, 3};
    EXPECT_EQ(m2x3.operator*(0), m2);

    //3x3
    m3x3.set({ {1, 4, 7}, {2, 5, 8}, {3, 6, 9} });
    Matrix m3{3, 3};
    m3.set({ {1.5, 6, 10.5}, {3, 7.5, 12}, {4.5, 9, 13.5} });
    EXPECT_EQ(m3x3.operator*(1.5), m3);

    Matrix m4{3, 3};
    m4.set({ {-2, -8, -14}, {-4, -10, -16}, {-6, -12, -18} });
    EXPECT_EQ(m3x3.operator*(-2), m4);

    Matrix m5{3, 3};
    EXPECT_EQ(m3x3.operator*(0), m5);
}

TEST_F(DifferentSizeMatrices, MatrixSolveEquation)
{
    // Determinant zero
    m3x3.set({ {1, 4, 7}, {2, 5, 8}, {3, 6, 9} });
    EXPECT_ANY_THROW(m3x3.solveEquation({3, 2, 3}));

    // Too big param vector
    m3x3.set({ {3, 2, 3}, {4, 5, 6}, {7, 8, 9} });
    EXPECT_ANY_THROW(m3x3.solveEquation({3, 2, 3, 4}));

    // 1x1 (singular)
    EXPECT_ANY_THROW(m1x1.solveEquation({3}));

    // 1x1
    m1x1.set({ {1} });
    std::vector<double> result_1x1 = {3};
    EXPECT_EQ(m1x1.solveEquation({3}), result_1x1);

    // 2x3 (checkSquare
    EXPECT_ANY_THROW(m2x3.solveEquation({3, 2, 3}));

    std::vector<double> result_3x3 = {1, -2, 4.0/3.0};
    EXPECT_EQ(m3x3.solveEquation( {3, 2, 3,} ), result_3x3);

    Matrix m1{4, 4};
    m1.set({
        {3, 2, 3, 2},
        {4, 5, 6, 2},
        {7, 8, 9, 2},
        {1, 1, 1, 2}
    });
    std::vector<double> result_4x4 = {1, -1.0/3.0, -1.0/3.0, 5.0/6.0};
    EXPECT_EQ(m1.solveEquation( {3, 2, 3, 2} ), result_4x4);
}

TEST_F(DifferentSizeMatrices, MatrixTranspose)
{
    m2x2.set({ {1, 2}, {8, 9} });
    Matrix m1{2, 2};
    m1.set({ {1, 8}, {2, 9} });
    EXPECT_EQ(m2x2.transpose(), m1);

    m2x3.set({ {1, 2, 3}, {6, 5, 4} });
    Matrix m2{3, 2};
    m2.set({ {1, 6}, {2, 5}, {3, 4} });
    EXPECT_EQ(m2x3.transpose(), m2);

    m3x2.set({ {1, 4}, {2, 5}, {3, 6}});
    Matrix m3{2, 3};
    m3.set({ {1, 2, 3}, {4, 5, 6} });
    EXPECT_EQ(m3x2.transpose(), m3);

    m3x3.set({ {1, 4, 7}, {2, 5, 8}, {3, 6, 9} });
    Matrix m4{3, 3};
    m4.set({ {1, 2, 3}, {4, 5, 6}, {7, 8, 9} });
    EXPECT_EQ(m3x3.transpose(), m4);
}

TEST_F(DifferentSizeMatrices, MatrixInverse)
{
    EXPECT_ANY_THROW(m3x2.inverse());
    EXPECT_ANY_THROW(m2x3.inverse());
    Matrix m1x1{1, 1};
    EXPECT_ANY_THROW(m1x1.inverse());
    Matrix m4x4{4, 4};
    EXPECT_ANY_THROW(m4x4.inverse());

    m2x2.set({ {1, 2}, {8, 9} });
    Matrix m1{2, 2};
    m1.set({ {-9.0/7.0, 2.0/7.0}, {8.0/7.0, -1.0/7.0} });
    EXPECT_EQ(m2x2.inverse(), m1);

    m3x3.set({ {1, 4, 7}, {2, 5, 8}, {3, 6, 9} });
    EXPECT_ANY_THROW(m3x3.inverse());

    m3x3.set({ {1, 2, 3}, {6, 5, 4}, {3, 2, 5} });
    Matrix m2{3, 3};
    m2.set({
        {-17.0/28.0, 1.0/7.0, 1.0/4.0},
        {9.0/14.0, 1.0/7.0, -1.0/2.0},
        {3.0/28.0, -1.0/7.0, 1.0/4.0}
        });
    EXPECT_EQ(m3x3.inverse(), m2);
}

/*** Konec souboru white_box_tests.cpp ***/
