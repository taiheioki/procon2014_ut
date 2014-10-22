// util::matrix unit test

#include <iostream>

#include "util/matrix.hpp"

#include <boost/optional.hpp>

#define BOOST_TEST_MODULE UtilMatrixTest
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
//#include <boost/test/included/unit_test.hpp>
#include <boost/test/unit_test.hpp>


#define CHECK_INV(C,N)\
    do{\
        for(int i = 0; i < (N); i++){\
            for(int j = 0; j < (N); j++){\
                if(i == j){\
                    BOOST_CHECK_CLOSE(C[i*N+j], 1.0, 1e-6);\
                }else{\
                    BOOST_CHECK_SMALL(C[i*N+j], 1e-6);\
                }\
            }\
        }\
    }while(0)

#define CHECK(A,B,C,N)\
    do{\
        bool r = inv_mat<N>(A,B);\
        BOOST_ASSERT(r);\
        mult(A, B, C, N, N, N);\
        CHECK_INV(C,N);\
    }while(0)

using namespace util::matrix;

BOOST_AUTO_TEST_SUITE(matrix)

BOOST_AUTO_TEST_CASE(one_test)
{
    BOOST_TEST_MESSAGE("1x1 test");
    double A[1] = {9};
    double B[1];
    double C[1];

    bool r = inv_mat<1>(A,B);
    BOOST_ASSERT(r);
    mult(A, B, C, 1, 1, 1);

    CHECK_INV(C,1);
}

BOOST_AUTO_TEST_CASE(two_test)
{
    BOOST_TEST_MESSAGE("2x2 test");

    double A1[4] = {3, 8, 9, 10};
    double A2[4] = {201, 0, -90, 50};
    double A3[4] = {12, 33, -89, -90};
    double B[4];
    double C[4];

    CHECK(A1,B,C,2);
    CHECK(A2,B,C,2);
    CHECK(A3,B,C,2);
}

BOOST_AUTO_TEST_CASE(three_test)
{
    BOOST_TEST_MESSAGE("3x3 test");

    double A1[9] = { 3,  8,  9,
                    19, 89, 12,
                    12, -39, 44};
    double A2[9] = { -10,  12, 123,
                    -50, 89, 222,
                    21, -9, 232};
    double A3[9] = { -111,  1021, 332,
                    -1001, 554, 24,
                    4004, 2999, 344};
    double B[9];
    double C[9];

    CHECK(A1,B,C,3);
    CHECK(A2,B,C,3);
    CHECK(A3,B,C,3);
}

BOOST_AUTO_TEST_CASE(four_test)
{
    BOOST_TEST_MESSAGE("4x4 test");

    double A1[16] = { 3,  8,  9, 20,
                    19, 89, 12, 22,
                    12, -39, 44,-20,
                    28, -55, -44, 0
                    };
    double A2[16] = { -10,  12, 123, 292
                    -50, 89, 222, 991,
                    21, -9, 232, 0,
                    112, 398, -56,32
                    };
    double A3[16] = { -111,  1021, 332, -234
                    -1001, 554, 24, 34,
                    4004, 2999, 344,1,
                    89, 12, 992, -102
                    };
    double B[16];
    double C[16];

    CHECK(A1,B,C,4);
    CHECK(A2,B,C,4);
    CHECK(A3,B,C,4);
}

BOOST_AUTO_TEST_SUITE_END()

